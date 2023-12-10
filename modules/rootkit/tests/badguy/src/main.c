#include <stdlib.h>
#include <stdio.h>

#include <args.h>
#include <kill.h>
#include <print.h>
#include <read.h>

void print_subcommands() {
	const char *subcommands[] = { "kill", NULL };
	print_error("Couldn't get subcommand");
	print_list("Here are the available subcommands: ", subcommands);
}

int main(int argc, char **argv) {	
	print_greeting();
	if (argc == 1 || argv[1] == NULL) {
		print_subcommands();
		return EXIT_FAILURE;
	}

	int subcommand = parse_subcommand(argv[1]);	
	
	t_kill_args kill_args;
	char *action;
	switch (subcommand) {
	case 0:
		kill_args = parse_kill_args(argc - 1, argv + 1);
		kill_run(kill_args.pid, kill_args.action);
		break;
	case 1:
		action = parse_read_args(argc -1, argv + 1);
		read_run((const char *)action);
		break;
	default:
		print_error("Incorrect subcommand.");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
