#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/cdev.h>
#include "scull.h"

int g_iScull_major =   SCULL_MAJOR;
int g_iScull_minor =   0;
int g_iScull_nr_devs = SCULL_NR_DEVS;	/* number of bare scull devices */

module_param(g_iScull_major, int, S_IRUGO);
module_param(g_iScull_minor, int, S_IRUGO);
module_param(g_iScull_nr_devs, int, S_IRUGO);

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
    .llseek = scull_llseek,
    .open = scull_open,
    .release = scull_release,
};

static void scull_setup_cdev(struct scull_dev *pDev, int iIndex)
{
    int err;
    unsigned uiDevNo = MKDEV(g_iScull_major, g_iScull_minor + iIndex);

    cdev_init(&pDev->cdev, &scull_fops);
    pDev->cdev.owner = THIS_MODULE;
    pDev->cdev.ops = &scull_fops;
    err = cdev_add(&pDev->cdev, uiDevNo, 1);
    if(err)
        printk(KERN_NOTICE "Error %d adding scull%d", err, iIndex);
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
