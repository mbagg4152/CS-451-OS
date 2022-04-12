#include <zconf.h>
#include <wait.h>
#include <stdio.h>
int main() {
	int i = 0;
	i++;

	if (fork() == 0) {
		fork();
		i++;
	} else {
		wait(NULL);
		fork();
		execl("/bin/echo", "echo", "I love forking!", NULL);
		i++;
	}
	printf("CS451\n");
	printf("i is %d \n", i);
	return 1;
}
