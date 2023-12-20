#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	int fd = 2600;
	size_t sz = 64;
	char buf[64];
	
	read(fd, buf, sz);

	return EXIT_SUCCESS;
}
