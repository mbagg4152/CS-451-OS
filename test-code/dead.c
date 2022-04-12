#include <stddef.h>
#include <zconf.h>
#include <fcntl.h>
#include <stdio.h>
int main(int argc, char *argv[]) {
	argv[0] = "/bin/cat";
	argv[1] = "temp.txt";
	argv[2] = (char *) NULL;
	if (fork() == 0) {
		int fd = open(argv[1], O_CREAT | O_WRONLY, 0664);
		if (fd != -1) {
			write(fd, "I like Operating Systems\n", 25);
			close(fd);
		}
	} else {
		execv(argv[0], argv);
	}
	printf("The shell is dead!\n");
	return 1;
}