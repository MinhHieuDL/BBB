#ifndef _ULOG_H_
#define _ULOG_H_

#include <linux/kernel.h>

#undef ULOG
#ifdef SCULL_DEBUG
    #ifdef __KERNEL__
        /* This one if debugging is on, and kernel space*/
        #define ULog(fmt, ...) \
            printk(KERN_DEBUG "%s: " fmt, __func__, ##__VA_ARGS__)
    #else
        /* This one for user space */
        #define ULog(fmt, ...) \
            fprintf(stderr, "%s: " fmt, __func__, ##__VA_ARGS__)
    #endif //__KERNEL__
#else
    #define ULog(fmt, args...) /* not debugging: nothing */
#endif  //SCULL_DEBUG

#endif  // _ULOG_H_