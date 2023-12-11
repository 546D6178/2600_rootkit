#include <termcolor-c.h>
#include <stdio.h>

void print_error(const char *error) { 
	fputs("Error: ", text_red(stdout));
	reset_colors(stdout);
	puts(error);
};

void print_info(const char *info) { 
	fputs("Info: ", text_blue(stdout));
	reset_colors(stdout);

	puts(info);
}

void print_success(const char *success) { 
	fputs("Success: ", text_green(stdout));
	reset_colors(stdout);

	puts(success);
}

void print_list(const char* what, const char *list[]) {
	fputs(what, text_blue(stdout));
	reset_colors(stdout);

	for (int i = 0; list[i] != NULL; i += 1) {
		fputs(list[i], text_green(stdout));
		reset_colors(stdout);

		if (list[i + 1] != NULL) fputs(", ", stdout);
	}
	puts("");
}

void print_greeting() {
	printf("Welcome to ");
	fputs("Badguy", text_red(stdout));
	reset_colors(stdout);
	fputs(", Rootkit 14's ", stdout);
	fputs("companion app\n", text_blue(stdout));
	reset_colors(stdout);
}

void print_bool(const char* str, int val) {
	printf("%s ", str);
	if (val != 0) {
		fputs("Yes", text_green(stdout));
	} else {
		fputs("No", text_red(stdout));
	}
	
	puts("");
	reset_colors(stdout);
}
