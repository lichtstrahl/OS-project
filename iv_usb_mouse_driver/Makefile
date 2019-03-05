obj-m += iv_usb_mouse_driver.o

KBUILD_EXTRA_SYMBOLS := Module.symvers

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
