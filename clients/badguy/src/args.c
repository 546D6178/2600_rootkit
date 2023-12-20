#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <args.h>
#include <print.h>

#include <kill.h>
#include <read.h>
#include <hide.h>

int parse_subcommand(const char* arg, const char **subcommands){
	for (int i = 0; subcommands[i] != NULL; i += 1) {
		size_t len = strnlen(subcommands[i], 64);
		if (!strncmp(arg, subcommands[i], len)) {
			return i;
		}
	}
	return -1;
}

t_kill_args parse_kill_args(int argc, char **argv) {
	int pid = 0;
	char *action = NULL;
	if (argv[1] != NULL) {
		action = argv[1];
	}
	
	if (argv[2] != NULL) {
		pid = atoi(argv[2]);
	} else {
		print_info("PID not specified, using 0");
	}

	t_kill_args args = { action, pid };
	return args;
}

char *parse_read_args(int argc, char **argv) {
	if (argv[1] != NULL) {
		return argv[1];
	}

	return NULL;
}

t_hide_args parse_hide_args(int argc, char **argv) {
	char *action = NULL;
	char *path = NULL;
	if (argv[1] != NULL) {
		action = argv[1];
	}	
	if (argv[2] != NULL) {
		path = argv[2];
	} else {
		print_error("Path not specified");
		exit(-1);
	}

	t_hide_args args = { action, path };
	return args;
}

int run(int argc, char **argv) {
	print_greeting();
	const char *subcommands[] = { "kill", "read", "hide", NULL };
	if (argc == 1 || argv[1] == NULL) {
		print_error("Couldn't get subcommand.");
		print_list("Here are the available subcommands: ", subcommands);
		return EXIT_FAILURE;
	}
	
	if (!strncmp(argv[1], "help", 5)) {
		char usage[64] = "";
		snprintf(usage, 64, "Usage - %s [subcommand] [action] <arguments>", argv[0]);
		print_info(usage);
		return EXIT_SUCCESS;
	}
	
	int subcommand = parse_subcommand(argv[1], subcommands);	
	
	t_kill_args kill_args;
	t_hide_args hide_args;
	char *read_action;
	switch (subcommand) {
	case 0:
		kill_args = parse_kill_args(argc - 1, argv + 1);
		kill_run(kill_args.action, kill_args.pid);
		break;
	case 1:
		read_action = parse_read_args(argc - 1, argv + 1);
		read_run(read_action);
		break;
	case 2:
		hide_args = parse_hide_args(argc - 1, argv + 1);
		hide_run(hide_args.action, hide_args.path);
		break;
	default:
		print_error("Incorrect subcommand.");
		print_list("Here are the available subcommands: ", subcommands);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
