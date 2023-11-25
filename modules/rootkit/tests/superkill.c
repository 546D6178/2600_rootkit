#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#define SIGSUPER 64

int main(int argc, char **argv) {
	if (argc != 3) return EXIT_FAILURE;
	
	int pid = atoi(argv[1]);
	int sig = atoi(argv[2]);

	int ret = kill(pid, sig);
	
	printf("kill(%d, %d) = %d\n", pid, sig, ret);
	
	return EXIT_SUCCESS;
}
