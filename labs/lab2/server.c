/******************************************************************************
 * Maggie Horton
 * CS-451 Summer 2020
 * Lab 2, Task 2
 * server.c
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
	printf("Hello and welcome to the server program for Lab2.\n"
		   "For this to work, make sure that you have started the"
		   " client in\nanother terminal.\nThis chat server-client"
		   " system is set up so that each must wait\nfor the others"
		   " response.\nIf the client enters 'quit' at the prompt"
		   " then both program loops\nstop.\n"
		   "==================================================================\n\n");

	// will hold the fifo descriptors
	int client_writing_fd;
	int server_writing_fd;

	// will hold data being sent & received via. the fifos
	char data_to_client[MAX_LEN];
	char data_from_client[MAX_LEN];

	// ensure old fifos are deleted
	unlink(CLNT_WRITING);
	unlink(SERV_WRITING);

	// make the fifos. get values in order to check for errors
	int clnt_fifo = mkfifo(CLNT_WRITING, PERM);
	int serv_fifo = mkfifo(SERV_WRITING, PERM);

	// mkfifo returns 0 on success, check for failure
	if (clnt_fifo || serv_fifo) {
		perror("error making fifo(s)\n");
		exit(1);
	}

	int comp = 1; // loop flag changed by string matching client input
	while (comp != 0) {
		// open the fifos using the paths
		client_writing_fd = open(CLNT_WRITING, O_RDONLY);
		server_writing_fd = open(SERV_WRITING, O_WRONLY);

		printf("You: ");
		// get data to send to client
		fgets(data_to_client, MAX_LEN, stdin);
		// write data to server write fifo
		write(server_writing_fd, data_to_client, MAX_LEN);

		// read in data from client write fifo
		read(client_writing_fd, data_from_client, MAX_LEN);

		// check if user entered quit
		comp = strcmp(data_from_client, "quit\n");
		if (comp == 0) {
			printf("\nClient decided to quit\n");
			break;
		}
		// print message from client
		printf("\n\t\t\t Client: %s", data_from_client);
	}
	// close fifo descriptors
	close(client_writing_fd);
	close(server_writing_fd);

	return 0;
}
