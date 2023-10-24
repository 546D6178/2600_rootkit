// hello.c kernel module

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

static __init int hello_init(void)
{
	printk(KERN_INFO "Module [hello] loaded\n");
	return 0;
}

static __exit void hello_exit(void){
	printk(KERN_INFO "Module [hello] unloaded\n");
	return;
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Useless ass module that doesn't do shit");
MODULE_AUTHOR("Salim Dardouchi");
