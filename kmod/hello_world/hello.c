#include <linux/module.h>
#include <linux/kernel.h> 
#include <linux/init.h>

static int __init hello_init(void)
{
    printk(KERN_INFO "hello: module loaded on BBB\n");
    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "hello: module unloaded\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mhle");
MODULE_DESCRIPTION("First kernel module");  
