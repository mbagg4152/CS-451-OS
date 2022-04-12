/******************************************************************************
 * Maggie Horton
 * CS-451 Summer 2020
 * Lab 2, Task 2
 * client.c
 ******************************************************************************/
#include <fcntl.h>
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define CLNT_WRITING "/tmp/clnt_wr" // fifo path, client writes & server reads
#define SERV_WRITING "/tmp/srvr_wr" // fifo path, server writes & client reads
#define MAX_LEN  100
#define PERM 0666


/******************************************************************************
 * Function: main
 * Details: runs the server fifo loop
 ******************************************************************************/
int main(int argc, char *argv[]) {
	printf("Hello and welcome to the client program for Lab2.\n"
		   "For this to work, make sure that you have started the\n"
		   "server in another terminal. If you started this one first,"
		   " close &\nthen restart, running server first.\nThis chat server-client"
		   " system is set up so that each must wait for\nthe others"
		   " response.\nEnter 'quit' to end both program loops.\n"
		   "==================================================================\n\n");

	// will hold the fifo descriptors
	int client_writing_fd;
	int server_writing_fd;

	// will hold data being sent & received via. the fifos
	char data_to_server[MAX_LEN];
	char data_from_server[MAX_LEN];


	int comp = 1; // loop flag changed by string matching client input
	while (comp != 0) {
		// open the fifos using the paths
		client_writing_fd = open(CLNT_WRITING, O_WRONLY);
		server_writing_fd = open(SERV_WRITING, O_RDONLY);

		// server is writing first so look for data from the server write fifo
		read(server_writing_fd, data_from_server, MAX_LEN);
		printf("\n\t\t\t Server: %s", data_from_server);

		printf("You: ");
		// get data to send to server
		fgets(data_to_server, MAX_LEN, stdin);
		// check if user entered quit
		comp = strcmp(data_to_server, "quit\n");
		if (comp == 0) {
			printf("\nYou decided to leave.\n");
			write(client_writing_fd, data_to_server, MAX_LEN);
			break;
		}
		// write message to server
		write(client_writing_fd, data_to_server, MAX_LEN);
	}
	// close fifo descriptors
	close(client_writing_fd);
	close(server_writing_fd);

	return 0;
}



