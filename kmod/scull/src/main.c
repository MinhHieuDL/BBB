#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/cdev.h>
#include <linux/slab.h>     /* kmalloc() */
#include <linux/errno.h>    /* error codes */
#include <uapi/asm-generic/errno-base.h>
#include <linux/kernel.h>   /* printk() */
#include <linux/uaccess.h>	/* copy_*_user */
#include "scull.h"
#include "ulog.h"

int g_iScull_major =   SCULL_MAJOR;
int g_iScull_minor =   0;
int g_iScull_nr_devs = SCULL_NR_DEVS;	/* number of bare scull devices */
int g_iScull_quantum = SCULL_QUANTUM;
int g_iScull_qset = SCULL_QSET;

module_param(g_iScull_major, int, S_IRUGO);
module_param(g_iScull_minor, int, S_IRUGO);
module_param(g_iScull_nr_devs, int, S_IRUGO);
module_param(g_iScull_quantum, int, S_IRUGO);
module_param(g_iScull_qset, int, S_IRUGO);

MODULE_AUTHOR("mhle");
MODULE_LICENSE("Dual BSD/GPL");

struct scull_dev *g_pScullDev;

int scull_trim(struct scull_dev *pDev)
{
    struct scull_qset *pNextQset, *pCurQSet;
    int iQsetSize = pDev->m_iQset;
    for(pCurQSet = pDev->m_pData; pCurQSet; pCurQSet = pNextQset)
    {
        if(pCurQSet->m_ppData)
        {
            for(int i = 0; i < iQsetSize; i++)
            {
                if(pCurQSet->m_ppData[i])
                    kfree(pCurQSet->m_ppData[i]);
            }
            kfree(pCurQSet->m_ppData);
            pCurQSet->m_ppData = NULL;
        }
        pNextQset = pCurQSet->m_pNext;
        kfree(pCurQSet);
    }
    pDev->m_ulSize = 0;
    pDev->m_pData = NULL;
    return 0;
}

struct scull_qset* scull_follow(struct scull_dev *pScullDev, int iIndex)
{
    struct scull_qset* pRet = pScullDev->m_pData;
    // Allocate first qset explicity if need be
    if(pRet == NULL)
    {
        pRet = pScullDev->m_pData = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
        if(pRet == NULL)
            return NULL;
        memset(pRet, 0, sizeof(struct scull_qset));
    }

    while(iIndex != 0)
    {
        pRet = pRet->m_pNext;
        if(pRet == NULL)
        {
            pRet = pScullDev->m_pData = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
            if(pRet == NULL)
                return NULL;
            memset(pRet, 0, sizeof(struct scull_qset));
        }
        iIndex -= 1;
    }
    return pRet;
}

ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    struct scull_dev *pDev = filp->private_data;
    struct scull_qset *pScullQset;

    int iQuantumSize = pDev->m_iQuantum;
    int iQSetSize = pDev->m_iQset;
    int iQsetDataSize = iQuantumSize * iQSetSize; 
    int iItemNum, iRest, iArrPos, iBytePos;
    
    ssize_t retValue = 0;
    ULog("read device\n");
    if(down_interruptible(&pDev->m_sema))
        return -ERESTARTSYS;
    if(*f_pos >= pDev->m_ulSize)
        goto out;
    if(*f_pos + count > pDev->m_ulSize)
        count = pDev->m_ulSize - (*f_pos);

    // find q_set number, array position, byte position to read
    iItemNum = (long)*f_pos / iQsetDataSize;
    iRest    = (long)*f_pos % iQsetDataSize;
    iArrPos  = iRest / iQuantumSize;
    iBytePos = iRest % iQuantumSize;

    // get the pointer to scull qset at item number
    pScullQset = scull_follow(pDev, iItemNum);

    if(!pScullQset || !pScullQset->m_ppData || !pScullQset->m_ppData[iArrPos])
        goto out;

    // Read only up to the end of current quantum
    count = (count > iQuantumSize - iBytePos) ? iQuantumSize - iBytePos : count; 

    if(copy_to_user((void*)buf, pScullQset->m_ppData[iArrPos] + iBytePos, count) != 0)
    {
        retValue = -EFAULT;
        goto out;
    }
    *f_pos += count; // update file position pointer
    retValue = count;

    out:
        up(&pDev->m_sema);
        return retValue;
}

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    struct scull_dev *pDev = filp->private_data;
    struct scull_qset *pScullQset;

    int iQuantumSize = pDev->m_iQuantum;
    int iQSetSize = pDev->m_iQset;
    int iQsetDataSize = iQuantumSize * iQSetSize; 
    int iItemNum, iRest, iArrPos, iBytePos;
    
    ssize_t retValue = -ENOMEM;
    
    ULog("write invoked with count %zu at position %lld\n", count, *f_pos);
    if(down_interruptible(&pDev->m_sema))
        return -ERESTARTSYS;
        
    // find q_set number, array position, byte position to read
    iItemNum = (long)*f_pos / iQsetDataSize;
    iRest    = (long)*f_pos % iQsetDataSize;
    iArrPos  = iRest / iQuantumSize;
    iBytePos = iRest % iQuantumSize;

    ULog("iItemNum: %d - iRest: %d - iArrPos: %d - iBytePos: %d", \
                    iItemNum, \
                    iRest,    \
                    iArrPos,  \
                    iBytePos);
    // get the position to write
    pScullQset = scull_follow(pDev,iItemNum);
    if(pScullQset == NULL)
        goto out;
    
    if(pScullQset->m_ppData == NULL)
    {
        pScullQset->m_ppData = kmalloc(iQSetSize * sizeof(char *), GFP_KERNEL);
        if(pScullQset->m_ppData == NULL)
        {
            ULog("Allocate pointers to Quantum data failed!\n");
            goto out;
        }
        memset(pScullQset->m_ppData, 0, iQSetSize * sizeof(char *));
    }

    if(pScullQset->m_ppData[iArrPos] == NULL)
    {
        pScullQset->m_ppData[iArrPos] = kmalloc(iQuantumSize, GFP_KERNEL);
        if(pScullQset->m_ppData[iArrPos] == NULL)
        {
            ULog("Allocate  Quantum data failed\n");
            goto out;
        }
        memset(pScullQset->m_ppData[iArrPos], 0, iQuantumSize);
    }

    // copy data to user
    count = (count > iQuantumSize - iBytePos) ? (iQuantumSize - iBytePos) : count;

    if(copy_from_user(pScullQset->m_ppData[iArrPos] + iBytePos, (void*)buf, count) != 0)
    {
        retValue = -EFAULT;
        goto out;
    }
    *f_pos += count;
    retValue = count;

    // update qset size
    pDev->m_ulSize = (pDev->m_ulSize < *f_pos) ? *f_pos : pDev->m_ulSize;

    out:
        up(&pDev->m_sema);
        return retValue;
}

loff_t  scull_llseek(struct file *filp, loff_t off, int whence)
{
    struct scull_dev *pDev = filp->private_data;
    loff_t newpos;

    switch (whence)
    {
    case 0:  //SEEK_SET
        newpos = off;
        break;
    case 1: //SEEK_CUR
        newpos = filp->f_pos + off;
        break;
    case 2: //SEEK_END
        newpos = pDev->m_ulSize + off;
        break;
    default:
        return -EINVAL; 
    }
    if (newpos < 0) return -EINVAL;
	filp->f_pos = newpos;
    
    return newpos;
}

int scull_open(struct inode *inode, struct file *filp)
{
    struct scull_dev *pScullDev;
    ULog("Open device\n");
    
    pScullDev = container_of(inode->i_cdev, struct scull_dev, m_cdev);
    filp->private_data = pScullDev;

    return 0;
}

int scull_release(struct inode *inode, struct file *filp)
{
    ULog("Release invoked\n");
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

static void scull_cleanup_module(void)
{
    dev_t devNo = MKDEV(g_iScull_major, g_iScull_minor);
    
    ULog("exit module - major number is: %i\n", g_iScull_major);
    // remove all scull devices
    if(g_pScullDev)
    {
        for(int i = 0; i < g_iScull_nr_devs; i++)
        {
            scull_trim(g_pScullDev + i);
            cdev_del(&g_pScullDev[i].m_cdev);
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

    cdev_init(&pDev->m_cdev, &scull_fops);
    pDev->m_cdev.owner = THIS_MODULE;
    pDev->m_cdev.ops = &scull_fops;
    err = cdev_add(&pDev->m_cdev, iDevNo, 1);
    if(err)
        printk(KERN_NOTICE "Error %d adding scull%d", err, iIndex);
}

static int __init scull_init_module(void)
{
    int iResult;
    dev_t devNo = 0;

    ULog("Init module\n");
    // Get device number for device 
    if (g_iScull_major) {
        devNo = MKDEV(g_iScull_major, g_iScull_minor);
        iResult = register_chrdev_region(devNo, g_iScull_nr_devs, "scull"); 
    }
    else {
        iResult = alloc_chrdev_region(&devNo, g_iScull_minor, g_iScull_nr_devs, "scull");
		g_iScull_major = MAJOR(devNo);
    }

    ULog("Major Number: %d\n", g_iScull_major);

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
        g_pScullDev[i].m_iQuantum = g_iScull_quantum;
        g_pScullDev[i].m_iQset = g_iScull_qset;
        sema_init(&g_pScullDev[i].m_sema, 1);
        scull_setup_cdev(&g_pScullDev[i], i);
    }

    return 0; // succeed

    fail:
	    scull_cleanup_module();
	    return iResult;
}



module_init(scull_init_module);
module_exit(scull_cleanup_module);
