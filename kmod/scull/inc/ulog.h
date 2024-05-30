#ifndef _ULOG_H_
#define _ULOG_H_

#include <linux/kernel.h>

#define ULog(fmt, ...) \
    printk(KERN_INFO "%s: " fmt, __func__, ##__VA_ARGS__)

#endif