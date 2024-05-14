#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include "scull.h"

MODULE_AUTHOR("mhle");
MODULE_LICENSE("Dual BSD/GPL");

ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t retValue;
    return retValue;
}

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t retValue;
    return retValue;
}

loff_t  scull_llseek(struct file *filp, loff_t off, int whence)
{
    loff_t newpos;
    return newpos;
}

int scull_open(struct inode *inode, struct file *filp)
{
    return 0;
}

int scull_release(struct inode *inode, struct file *filp)
{
	return 0;
}

struct file_operations scull_fops = {
    .owner = THIS_MODULE,
    .read  = scull_read,
    .write = scull_write,
    .llseak = scull_llseek,
    .open = scull_open,
    .release = scull_release,
}

int scull_init_module(void)
{
    return 0;
}

void scull_cleanup_module(void)
{

}

module_init(scull_init_module);
module_exit(scull_cleanup_module);
