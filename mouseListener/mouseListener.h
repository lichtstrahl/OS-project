#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#include "mouseListenerExtern.h"

#define DIR_NAME "mouseListener"
#define NODE_NAME "info"
#define MAX_ID 99
#define MESSAGE_SIZE 14

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Igor Smirnov");

static struct proc_dir_entry *procDir;

static int __init moduleInit(void);
static void __exit moduleExit(void);

static int nodeOpen(struct inode *inode, struct file *file);
static ssize_t nodeRead(struct file *file, char *buf, size_t count, loff_t *ppos);
static int nodeClose(struct inode *inode, struct file *file);

static const struct file_operations nodeFops =
{
    .owner = THIS_MODULE,
    .open = nodeOpen,
    .read = nodeRead,
    .release = nodeClose
};

typedef enum mouseButton
{
    NONE,
    LEFT,
    RIGHT,
    MIDDLE,
    WHEELUP,
    WHEELDOWN,
    SIDE_BUTTON_1,
    SIDE_BUTTON_2,
} MouseButton;

MouseButton button;
int id = 0;
char *mouseInfoMsg;

module_init(moduleInit);
module_exit(moduleExit);
