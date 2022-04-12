/******************************************************************************
 * Maggie Horton
 * CS-451 Summer 2020
 * Lab 2, Task 1
 * myshell.c
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define CMD_LEN 20
#define ERR (-1)
#define FALSE 0
#define KEY  "quit"
#define MAX_LEN 90
#define TRUE 1

int in_dir(char *path);
void run_sort(char *cmd_name, char *txt_path, int write_flag, char *out_path);


/******************************************************************************
 * Function: main
 * Details: reads in user input and determines whether mysort's output should
 * 			be written to stdout or a file.
 ******************************************************************************/
int main(int argc, char **argv) {
	char input[MAX_LEN];

	char cmd_str[CMD_LEN] = "";
	char inp_file[CMD_LEN] = "";
	char direct[CMD_LEN] = "";
	char out_file[CMD_LEN] = "";
	int comp = 1;
	while (comp != 0) {
		printf("myprompt> ");
		fgets(input, MAX_LEN, stdin);
		sscanf(input, "%s %s %s %s", cmd_str, inp_file, direct, out_file);

		comp = strcmp(KEY, cmd_str);
		if (comp == 0) {
			printf("\nSee ya!\n");
			exit(0);
		}


		// look at user input to determine what to do

		if ((in_dir(cmd_str) == ERR) || strlen(cmd_str) <= 0) { // bad command name
			printf("\n%s is unrecognized as a command or program\n", cmd_str);
		} else if (in_dir(inp_file) == ERR || strlen(cmd_str) <= 0) { // bad input file name
			printf("\ncan't find input file '%s', try again\n", inp_file);
		} else if ((strcmp(direct, ">") == 0) && (strlen(out_file) <= 0)) { // no output file path after '>'
			printf("got empty value after '>'.\n");
		} else if ((strcmp(direct, ">") == 0) && strlen(out_file) > 0) { // output to be redirected to file
			run_sort(cmd_str, inp_file, TRUE, out_file);
		} else { // output will just be displayed to stdout
			printf("\n");
			run_sort(cmd_str, inp_file, FALSE, NULL);
			printf("\n");
		}
	}
}


/******************************************************************************
 * Function: run_sort
 * Details: Creates process to run mysort
 * Input: command name (char*), input text file path (char*), write to file
 * 		  flag (int) & output text file path (char*)
 * Output: Nothing
 ******************************************************************************/
void run_sort(char *cmd_name, char *txt_path, int write_flag, char *out_path) {
	pid_t pid = fork();
	char run_arg[CMD_LEN + 3] = "./";
	strcat(run_arg, cmd_name);

	if (pid == 0) { // the child
		// args to send to execvp
		char *args[] = {run_arg, txt_path, NULL};
		if (write_flag == TRUE) { // create output file
			if (in_dir(out_path)) remove(out_path);
			int output_fd = open(out_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
			// direct output from mysort to output file
			dup2(output_fd, STDOUT_FILENO);
			close(output_fd);
			// run mysort
			execvp(args[0], args);
		} else { // output from mysort stays in stdout
			execvp(args[0], args);
		}
	} else waitpid(pid, 0, 0);
}


/******************************************************************************
 * Function: in_dir
 * Details: Simple function to check if a file exists or is accessible.
 * Input: Path of file to be checked (char*)
 * Output: Result code (int)
 ******************************************************************************/
int in_dir(char *path) {
	if (access(path, F_OK) != ERR) return TRUE;
	else return ERR;
}

