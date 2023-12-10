#include <string.h>
#include <stdlib.h>

#include <args.h>
#include <print.h>
int parse_subcommand(const char* arg){
	if (!strncmp("kill", arg, 5)) {
		return 0;
	} else if (!strncmp("read", arg, 5)) {
		return 1;
	}
	return -1;
}

t_kill_args parse_kill_args(int argc, char **argv) {
	int pid = 0;
	char *action = NULL;
	if (argv[1] != NULL) {
		action = (char*)argv[1];
	}
	
	if (argv[2] != NULL) {
		pid = atoi(argv[2]);
	} else {
		print_info("PID not specified, using 0");
	}

	t_kill_args args = { pid, action };
	return args;
}

char *parse_read_args(int argc, char **argv) {
	if (argv[1] != NULL) {
		return argv[1];
	}

	return NULL;
}
