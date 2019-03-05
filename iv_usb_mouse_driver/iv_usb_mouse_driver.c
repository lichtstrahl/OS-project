#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

#include "../mouseListener/mouseListenerExtern.h"

/*
 * Version Information
 */
#define DRIVER_VERSION "v1.6"
#define DRIVER_AUTHOR "Igor Smirnov"
#define DRIVER_DESC "USB HID Boot Protocol mouse driver"
#define DRIVER_LICENSE "GPL"

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE(DRIVER_LICENSE);

struct usb_mouse {
        char name[128];
        char phys[64];
        struct usb_device *usbdev;
        struct input_dev *dev;
        struct urb *irq;
        signed char *data;
        dma_addr_t data_dma;
};

static void usb_mouse_irq(struct urb *urb)
{
        struct usb_mouse *mouse = urb->context;
        signed char *data = mouse->data;
        struct input_dev *dev = mouse->dev;
        int status;

        switch (urb->status) {
            case 0:   {              /* success */
                    if(mouseListenerSendCoordinates(data) == 0)
                        printk(KERN_INFO "My USB module:   Can't send data");

                    input_sync(dev);
                    break;
                }
            case -ECONNRESET:       /* unlink */
            case -ENOENT:
            case -ESHUTDOWN:
                    return;
        }

        // Драйвер обработал URB и готов отправить его в ядро, чтобы оно переправило его устройству
        // Указывается блок и способ выделения памяти
        // Поскольку данный код выполняется вне контекста процесса (это прерывание), то устанавливается соответствующий флаг
        status = usb_submit_urb (urb, GFP_ATOMIC);
        if (status)
                dev_err(&mouse->usbdev->dev,
                        "can't resubmit intr, %s-%s/input0, status %d\n",
                        mouse->usbdev->bus->bus_name,
                        mouse->usbdev->devpath, status);
}

static int usb_mouse_open(struct input_dev *dev)
{
        struct usb_mouse *mouse = input_get_drvdata(dev);
        mouse->irq->dev = mouse->usbdev;
        if (usb_submit_urb(mouse->irq, GFP_KERNEL))
                return -EIO;
        return 0;
}

static void usb_mouse_close(struct input_dev *dev)
{
        struct usb_mouse *mouse = input_get_drvdata(dev);

        usb_kill_urb(mouse->irq);
}

static int usb_mouse_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
        struct usb_device *dev = interface_to_usbdev(intf);
        struct usb_host_interface *interface;
        struct usb_endpoint_descriptor *endpoint;
        struct usb_mouse *mouse;
        struct input_dev *input_dev;
        int pipe, maxp;
        int error = -ENOMEM;

        interface = intf->cur_altsetting;

        if (interface->desc.bNumEndpoints != 1)
                return -ENODEV;
        
        // Получаем 0-ую конечную точку, которая гарантировано уже открыта у устройства
        endpoint = &interface->endpoint[0].desc;
        // Проверяем, что данная точка поддерживает приём прерываний
        if (!usb_endpoint_is_int_in(endpoint))
                return -ENODEV;
       
        // Создаётся входная конечная точка прерывания для указанного устройства, с указанным номером.
        // В Дальнейшем она будет использоваться для инициализации URB
        // Данная точка работает на приём, так как мы лишь принимаем сигналы от устройства
        pipe = usb_rcvintpipe(dev, endpoint->bEndpointAddress);
        
        
        // узнаём максимальный размер пакета. Буфера.
        // usb_pipeout с помощью конкретных битов в pipe определяют, являеется он in/out
        maxp = usb_maxpacket(dev, pipe, usb_pipeout(pipe));
        
        // Выделение памяти под mouse
        mouse = kzalloc(sizeof(struct usb_mouse), GFP_KERNEL);
        // Выделение памяти для нового устройства ввода
        input_dev = input_allocate_device();
        if (!mouse || !input_dev) {
            input_free_device(input_dev);
            kfree(mouse);
            return error;
        }

        // Выделение памяти под буфер. Рекомендуется использовать для USB-устройств
        // Необходим для того, чтобы не накапливались копии одних и тех же данных
        // В общем обычный kmalloc, но с использованием DMA - прямого доступа к памяти
        // Передается устройство, размер буффера, способ выделения и адрес DMA буфера
        mouse->data = usb_alloc_coherent(dev, 8, GFP_ATOMIC, &mouse->data_dma);
        if (!mouse->data) {
            input_free_device(input_dev);
            kfree(mouse);
            return error;
        }
        
        // Выделяется мето под URB, GFP_KERNEL значит, 
        // что выделение происходит от имени процесса
        // 0 - число изохронных пакетов
        mouse->irq = usb_alloc_urb(0, GFP_KERNEL);
        if (!mouse->irq) {
            usb_free_coherent(dev, 8, mouse->data, mouse->data_dma);

            input_free_device(input_dev);
            kfree(mouse);
            return error;
        }

        mouse->usbdev = dev;
        mouse->dev = input_dev;

        if (dev->manufacturer)
                strlcpy(mouse->name, dev->manufacturer, sizeof(mouse->name));

        if (dev->product) {
                if (dev->manufacturer)
                        strlcat(mouse->name, " ", sizeof(mouse->name));
                strlcat(mouse->name, dev->product, sizeof(mouse->name));
        }

        if (!strlen(mouse->name))
                snprintf(mouse->name, sizeof(mouse->name),
                         "USB HIDBP Mouse %04x:%04x",
                         le16_to_cpu(dev->descriptor.idVendor),
                         le16_to_cpu(dev->descriptor.idProduct));

        usb_make_path(dev, mouse->phys, sizeof(mouse->phys));
        strlcat(mouse->phys, "/input0", sizeof(mouse->phys));
        
        input_dev->name = mouse->name;
        input_dev->phys = mouse->phys;
        usb_to_input_id(dev, &input_dev->id);
        input_dev->dev.parent = &intf->dev;

        // С помощью битовых масок происходит привязка кнопок мыши
        // Учитываются дополнительные клавиши
        input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
        input_dev->keybit[BIT_WORD(BTN_MOUSE)] = BIT_MASK(BTN_LEFT) |
                BIT_MASK(BTN_RIGHT) | BIT_MASK(BTN_MIDDLE);
        input_dev->relbit[0] = BIT_MASK(REL_X) | BIT_MASK(REL_Y);
        input_dev->keybit[BIT_WORD(BTN_MOUSE)] |= BIT_MASK(BTN_SIDE) |
                BIT_MASK(BTN_EXTRA);
        input_dev->relbit[0] |= BIT_MASK(REL_WHEEL);
        // Установка driver_data у input_dev
        // При этом сам input_dev уже связан с переданным интерфейсом
        input_set_drvdata(input_dev, mouse);
        
        // Функции, вызываемые при подключении и отключении устройства
        input_dev->open = usb_mouse_open;
        input_dev->close = usb_mouse_close;
        
        /*
                Инициализация выделенного urb
                dev - устройство, которому будет отправлен urb
                pipe - конечная точка (создана ранее)
                data - буффер для приёма входящих сообщений
                maxp - размер буфера
                irq - завершающий обработчик, который вызовется при завершении этого urb
                mouse - контекст, используется в обработчике
                interval - интервал, с которым urb должен быть запланирован
        */
        usb_fill_int_urb(mouse->irq, dev, pipe, mouse->data,
                         (maxp > 8 ? 8 : maxp),
                         usb_mouse_irq, mouse, endpoint->bInterval);
        mouse->irq->transfer_dma = mouse->data_dma;
        mouse->irq->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
        
        // Регистрация нового устройства ввода
        error = input_register_device(mouse->dev);
        if (!error) {
            usb_set_intfdata(intf, mouse);
            return 0;
        }

        // Освобождение памяти под URB. В случае какой-то ошибки
        usb_free_urb(mouse->irq);

        usb_free_coherent(dev, 8, mouse->data, mouse->data_dma);

        input_free_device(input_dev);
        kfree(mouse);
        return error;
}

static void usb_mouse_disconnect(struct usb_interface *intf)
{
        // Данная функция получает поле driver_data
        // В конце функции probe мы уже положили туда структуру mouse
        struct usb_mouse *mouse = usb_get_intfdata (intf);
        // Зануляем информацию об устройстве
        usb_set_intfdata(intf, NULL);
        // Очищаем выделенные при работе ресурсы
        if (mouse) {
                usb_kill_urb(mouse->irq);
                input_unregister_device(mouse->dev);
                usb_free_urb(mouse->irq);
                usb_free_coherent(interface_to_usbdev(intf), 8, mouse->data, mouse->data_dma);
                kfree(mouse);
        }
}

static struct usb_device_id usb_mouse_id_table [] = {
        { USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT,
                USB_INTERFACE_PROTOCOL_MOUSE) },
        { }     /* Terminating entry */
};

// Макрос для создания карт. Нужен, чтобы драйвер автоматически загружался, если подключается подходящее устройство
MODULE_DEVICE_TABLE (usb, usb_mouse_id_table);

static struct usb_driver usb_mouse_driver = {
        .name           = "iv_usb_mouse_driver",
        .probe          = usb_mouse_probe,
        .disconnect     = usb_mouse_disconnect,
        .id_table       = usb_mouse_id_table,
};

// Стандартный макрос, чтобы не писать init/exit функции самостоятельно
module_usb_driver(usb_mouse_driver);
