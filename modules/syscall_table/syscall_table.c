#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maxence");
MODULE_DESCRIPTION("get addr open");




static int __init rootkit_init(void)
{

    typedef void *(*kallsyms_t)(const char *);
    kallsyms_t kallsyms_lookup_name;
    uint64_t *sysopen = 0;
 
    static struct kprobe probe = {
        .symbol_name = "kallsyms_lookup_name"
    };

	register_kprobe(&probe);
	printk(KERN_INFO "Found kallsyms_lookup_name at 0x%px \n", probe.addr); 

    kallsyms_lookup_name = (kallsyms_t) probe.addr;
    unregister_kprobe(&probe);
    sysopen = kallsyms_lookup_name("do_sys_open");
    if (sysopen) {
        printk(KERN_INFO "Found do_sys_open at 0x%px \n", sysopen); 
    }else{
        printk(KERN_ERR "[-] Failed to get kallsyms_lookup_name(dosysopen) address.");
    }
    return 0;
}

void __exit cleanup_rootkit(void) {
    return;
}

module_init(rootkit_init);
module_exit(cleanup_rootkit);
