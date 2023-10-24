#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#include <stdio.h>

#define VERSION_MAGIC		'k'

#define VERSION_RESET		_IO(VERSION_MAGIC, 0)
#define VERSION_MODIFIED 	_IO(VERSION_MAGIC, 1)

int main(int argc, char **argv){

	int version_fd = open("/dev/version", O_RDWR);
	
	char version[64];
	read(version_fd, version, 64);
	printf("[test] Current version %s\n", version);
		
	const char* stuff = "cacaboudin";
	write(version_fd, stuff, 64);
	printf("[test] Modified version for %s\n", stuff);
	
	int is_modified = ioctl(version_fd, VERSION_MODIFIED);
	printf("[test] is version modified ? %s\n", is_modified ? "yes" : "no");

	ioctl(version_fd, VERSION_RESET);	
	int is_reset = is_modified ^ ioctl(version_fd, VERSION_MODIFIED);

	printf("[test] reset version, is it the case ? %s\n", is_reset ? "yes" : "no");

	close(version_fd);	
	return 0;
}
