sudo rmmod usbhid
sudo insmod mouseListener/mouseListener.ko
sudo insmod usbmouse/usbmouse.ko
sudo `which python3` kb/main.py &

