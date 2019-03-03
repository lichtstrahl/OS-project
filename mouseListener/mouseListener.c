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
    printk(KERN_INFO "dataToButton: %s", msg);
    if (data[0] & 0x01)
    {
        return LEFT;
    }
    if (data[0] & 0x02)
    {
        return RIGHT;
    }
    if (data[0] & 0x04)
    {
        return MIDDLE;
    }
    if (data[6] == 1)
    {
        return WHEELUP;
    }
    if (data[6] == -1)
    {
        return WHEELDOWN;
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
    try_module_get(THIS_MODULE);
    sprintf(mouseInfoMsg, "%d\n%s\n", id, buttonToString());
    return 0;
}

static ssize_t nodeRead(struct file* file, char* buf, size_t count, loff_t* ppos)
{
    if(*ppos >= strlen(mouseInfoMsg))
    {
        *ppos = 0;
        return 0;
    }
    if(count > strlen(mouseInfoMsg) - *ppos)
    {
        count = strlen(mouseInfoMsg) - *ppos;
    }
    copy_to_user((void*) buf, mouseInfoMsg + *ppos, count);
    *ppos += count;
    return count;
}

static int nodeClose(struct inode* inode, struct file* file)
{
    module_put(THIS_MODULE);
    return 0;
}
