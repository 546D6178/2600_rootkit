#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#include <print.h>
#include <hide.h>

int check_slash(char *path) {
	for (size_t i = 0; path[i] != 0; i += 1) {
		if (path[i] == '/') return 1;
	}
	return 0;
}

char *craft_complete_path(char* path, char* basename) {
	size_t len = strlen(basename) + strlen(path) + strlen(HIDE_PREFIX) + 3; // +3 for the terminating zero, the slash and the underscore.
	char *dst_path = (char *)malloc(sizeof(char) * len);
	snprintf(dst_path, len, "%s/%s_%s", path, HIDE_PREFIX, basename);
	
	return dst_path;
}

char *separate_basename(char *path) {
	size_t len = strnlen(path, 256);	
	char *basename = NULL;

	if (!check_slash(path)) return path;
	for (size_t i = len; path[i] != '/'; i -= 1) {
		basename = path + i;
	}
	
	path[basename - path - 1] = 0;
		
	return basename;
}

int create_hidden_folder(char *path) {
	char *basename = separate_basename(path);

	char *dst_path;
	if (check_slash(path)) {
		dst_path = craft_complete_path(path, basename);
	} else {
		char cwd[1024];
		getcwd(cwd, 1024);
		dst_path = craft_complete_path(cwd, path);
	}

	int ret = mkdir(dst_path, 0700);
	chown(dst_path, 0, 0);
	if (ret == 0) {
		print_success("Successfully hid folder.");
		print_info(dst_path);
	} else {
		print_error("Couldn't hide folder.");
	}

	free(dst_path);
	
	return ret;
}

int create_hidden_file(char* path) {
	if (access(path, F_OK) != 0) {
		print_error("Couldn't find file.");
		return -1;
	}

	size_t path_len = strlen(path);
	char * src_path = (char *)malloc(sizeof(char) * path_len);
	strncpy(src_path, path, path_len);

	char *basename = separate_basename(path);
	
	char *dst_path;
	if (check_slash(path)) {
		dst_path = craft_complete_path(path, basename);
	} else {
		char cwd[1024];
		getcwd(cwd, 1024);
		dst_path = craft_complete_path(cwd, path);
	}

	int ret = rename(src_path, dst_path);

	if (ret == 0) {
		print_success("successfully created empty hidden file.");
		print_info(dst_path);
	} else {
		print_error("couldn't create hidden file.");
	}

	free(dst_path);
	free(src_path);

	return ret;	
}

int list_hidden(char *path) {
	return 0;
}

int hide_run(char *action, char *path) { 
	const char *actions[] = { "create_folder", "file", "list", NULL };
	if (action == NULL) {
		print_error("Hide action not specified.");
		print_list("Here are all the actions you can do: ", actions);
		return -2;
	}
	if (!strncmp(action, actions[0], 7)) {
		return create_hidden_folder(path); 
	} else if (!strncmp(action, actions[1], 5)) {
		return create_hidden_file(path);
	} else if (!strncmp(action, actions[2], 5)) {
			return list_hidden(path);
	}
	
	print_error("Incorrect action");
	print_list("Here are the possible actions: ", actions);
	return -1;
}
