#ifndef _SCULL_H_
#define _SCULL_H_

#include <linux/types.h>
#include <linux/fs.h>

#ifndef SCULL_MAJOR
#define SCULL_MAJOR 0   /* dynamic major by default */
#endif

#ifndef SCULL_NR_DEVS
#define SCULL_NR_DEVS 4    /* scull0 through scull3 */
#endif
struct scull_qset {
	void **data;
	struct scull_qset *next;
};

struct scull_dev {
    struct scull_qset *m_pData;
    int m_iQuantum;
    int m_iQset;
    unsigned long m_ulSize;
    unsigned int m_iAccessKey;
    struct cdev m_cdev;
};

/* Scull file operatios*/

ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
loff_t  scull_llseek(struct file *filp, loff_t off, int whence);

#endif