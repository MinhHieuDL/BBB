#ifndef _SCULL_H_
#define _SCULL_H_

#include <linux/types.h>
#include <linux/fs.h>

/* Scull file operatios*/

ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
                   loff_t *f_pos);
ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
loff_t  scull_llseek(struct file *filp, loff_t off, int whence);

#endif