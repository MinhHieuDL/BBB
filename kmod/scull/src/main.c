#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/cdev.h>
#include <linux/slab.h>  
#include <linux/errno.h>
#include "scull.h"

int g_iScull_major =   SCULL_MAJOR;
int g_iScull_minor =   0;
int g_iScull_nr_devs = SCULL_NR_DEVS;	/* number of bare scull devices */

module_param(g_iScull_major, int, S_IRUGO);
module_param(g_iScull_minor, int, S_IRUGO);
module_param(g_iScull_nr_devs, int, S_IRUGO);

MODULE_AUTHOR("mhle");
MODULE_LICENSE("Dual BSD/GPL");

struct scull_dev *g_pScullDev;

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

static void __exit scull_cleanup_module(void)
{
    dev_t devNo = MKDEV(g_iScull_major, g_iScull_minor);

    // remove all scull devices
    if(g_pScullDev)
    {
        for(int i = 0; i < g_iScull_nr_devs; i++)
        {
            cdev_del(&g_pScullDev[i].cdev);
        }
        kfree(g_pScullDev);
    }

    // unregister device number
    unregister_chrdev_region(devNo, g_iScull_nr_devs);
}

static void scull_setup_cdev(struct scull_dev *pDev, int iIndex)
{
    int err;
    int iDevNo = MKDEV(g_iScull_major, g_iScull_minor + iIndex);

    cdev_init(&pDev->cdev, &scull_fops);
    pDev->cdev.owner = THIS_MODULE;
    pDev->cdev.ops = &scull_fops;
    err = cdev_add(&pDev->cdev, iDevNo, 1);
    if(err)
        printk(KERN_NOTICE "Error %d adding scull%d", err, iIndex);
}

static int __init scull_init_module(void)
{
    int iResult;
    dev_t devNo = 0;

    // Get device number for device 
    if (g_iScull_major) {
        devNo = MKDEV(g_iScull_major, g_iScull_minor);
        iResult = register_chrdev_region(devNo, g_iScull_nr_devs, "scull"); 
    }
    else {
        iResult = alloc_chrdev_region(&devNo, g_iScull_minor, g_iScull_nr_devs, "scull");
		g_iScull_major = MAJOR(devNo);
    }

    if (iResult < 0) {
	    printk(KERN_WARNING "scull: can't get major %d\n", g_iScull_minor);
	    return iResult;
	}

    // allocate device
    g_pScullDev = kmalloc(g_iScull_nr_devs * sizeof(struct scull_dev), GFP_KERNEL);
    if(!g_pScullDev) {
        iResult = -ENOMEM;
		goto fail; 
    }
    memset(g_pScullDev, 0, g_iScull_nr_devs * sizeof(struct scull_dev));

    // init for each device
    for(int i = 0; i < g_iScull_nr_devs; i++) {
        scull_setup_cdev(&g_pScullDev[i], i);
    }

    return 0; // succeed

    fail:
	    scull_cleanup_module();
	    return iResult;
}



module_init(scull_init_module);
module_exit(scull_cleanup_module);
