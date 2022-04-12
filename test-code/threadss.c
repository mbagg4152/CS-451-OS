#include <stdio.h>
#include <zconf.h>
#include <pthread.h>
int balance = 0;
void *myThread(void *arg) {
	int i;
	for (i = 0; i < 200000; i++) {
		balance++;
	}
	printf("balance of %s is %d\n", (char *) arg, balance);
	return NULL;
}
int main(int argc, char *argv[]) {
	pthread_t p1, p2, p3;
	pthread_create(&p1, NULL, myThread, "A");
	pthread_join(p1, NULL);
	pthread_create(&p2, NULL, myThread, "B");
	myThread("main");
	pthread_join(p2, NULL);
	pthread_create(&p3, NULL, myThread, "C");
	pthread_join(p3, NULL);
	printf("final balance is %d\n", balance);
}