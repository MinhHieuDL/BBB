#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("Dual BSD/GPL");

static char* whom="Mom";
static int iHowMany = 10;
module_param(iHowMany, int, S_IRUGO);
module_param(whom, charp, S_IRUGO);

static int hello_init(void)
{
    for(int i = 0; i < iHowMany; i++)
        printk(KERN_ALERT "Hello, %s\n", whom);
    return 0;
}

static void hello_exit(void)
{
    printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
