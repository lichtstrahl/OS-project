#include "mouseListener.h"

static int __init moduleInit(void)
{
    if ((procDir = proc_mkdir_mode(DIR_NAME, S_IFDIR | S_IRWXUGO, NULL)) == NULL)
    {
        printk(KERN_INFO "+++Can't create dir /proc/%s", DIR_NAME);
        return -ENOENT;
    }
    if (proc_create_data(NODE_NAME, S_IFREG | S_IRUGO | S_IWUGO, procDir, &nodeFops, NULL) == NULL)
    {
        printk(KERN_INFO "+++Can't create node /proc/%s/%s", DIR_NAME, NODE_NAME);
        remove_proc_entry(DIR_NAME, NULL);
        return -ENOMEM;
    }
    printk(KERN_INFO "+++Node /proc/%s/%s installed", DIR_NAME, NODE_NAME);
    mouseInfoMsg = (char*) kmalloc(MESSAGE_SIZE, GFP_KERNEL);
    return 0;
}

static void __exit moduleExit(void)
{
    kfree(mouseInfoMsg);
    remove_proc_entry(NODE_NAME, procDir);
    remove_proc_entry(DIR_NAME, NULL);
}

static MouseButton dataToButton(signed char *data)
{
    char msg[512];
    sprintf(msg, "%d %d %d %d %d %d %d %d", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);

    if (data[0] & 0x01 && !data[2] && !data[3] && !data[4] && !data[5])
    {
        printk(KERN_INFO "dataToButton: %s", msg);
        return LEFT;
    }
    if (data[0] & 0x02 && !data[2] && !data[3] && !data[4] && !data[5])
    {
        printk(KERN_INFO "dataToButton: %s", msg);
        return RIGHT;
    }
    if (data[0] & 0x04 && !data[2] && !data[3] && !data[4] && !data[5])
    {
        printk(KERN_INFO "dataToButton: %s", msg);
        return MIDDLE;
    }
    if (data[6] == 1)
    {
        printk(KERN_INFO "dataToButton: %s", msg);
        return WHEELUP;
    }
    if (data[6] == -1)
    {
        printk(KERN_INFO "dataToButton: %s", msg);
        return WHEELDOWN;
    }
    if (data[0] == 8 && !data[2] && !data[3] && !data[4] && !data[5])
    {
        printk(KERN_INFO "dataToButton: %s", msg);
        return SIDE_BUTTON_1;
    }
    if (data[0] == 16 && !data[2] && !data[3] && !data[4] && !data[5])
    {
        printk(KERN_INFO "dataToButton: %s", msg);
        return SIDE_BUTTON_2;
    }
    return NONE;
}

static char *buttonToString(void)
{
    switch (button)
    {
        case LEFT:
            return "LEFT";
        case RIGHT:
            return "RIGHT";
        case MIDDLE:
            return "MIDDLE";
        case WHEELUP:
            return "WHEELUP";
        case WHEELDOWN:
            return "WHEELDOWN";
        case SIDE_BUTTON_1:
            return "SIDE_1";
        case SIDE_BUTTON_2:
            return "SIDE_2";
        default:
            return "NONE";
    }

    printk(KERN_INFO "+++buttonToString: %d", button);
}

extern bool mouseListenerSendCoordinates(signed char *data)
{
    MouseButton mb = dataToButton(data);
    if (mb != NONE)
    {
        button = mb;
        if (++id > MAX_ID) {
            id = 0;
        }
    }
    return 1;
}

EXPORT_SYMBOL(mouseListenerSendCoordinates);

static int nodeOpen(struct inode* inode, struct file* file)
{
    // Увеличиваем счетчик ссылок, чтобы между операцией открытия и чтения файла не произошла выгрузка модуля
    try_module_get(THIS_MODULE);
    return 0;
}

static ssize_t nodeRead(struct file* file, char* buf, size_t count, loff_t* ppos)
{
    sprintf(mouseInfoMsg, "%d\n%s\n", id, buttonToString());
    count = strlen(mouseInfoMsg);
    copy_to_user(buf, mouseInfoMsg, count);
    return count;
}

static int nodeClose(struct inode* inode, struct file* file)
{
    // После завершения работы с файлом уменьшаем количество ссылок
    module_put(THIS_MODULE);
    return 0;
}
