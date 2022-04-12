/******************************************************************************
 * Maggie Horton
 * CS-451
 * Summer 2020
 * Project 1, myshell.c: A simple shell program that can call 3 commands: my-cat,
 * 						 my-uniq & my-wc, which function similarly to cat, uniq
 * 						 & wc respectively. Can be used to do simple commands
 * 						 or can be used to pipe commands together (max of 2
 * 						 times).
 ******************************************************************************/
#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// arbitrary length limits
#define CMD_LEN 48          // size limit per command
#define MAX_ARG_LEN 5       // max length of array holding args
#define MAX_INP_LEN 240     // max length for input

// defs to determine how to handle pipes
#define NO_PIPE 2           // 2 args ==> cmd1 & path
#define SINGLE_PIPE 4       // 4 args ==> cmd1, path, pipe & cmd2
#define DOUBLE_PIPE 6       // 6 args ==> cmd1, path, pipe1, cmd2, pipe2 & cmd3

// defs used in implementing pipe functionality
#define CHILD 0             // 0 is pid of child process
#define READ 0              // == STDIN_FILENO
#define WRITE 1             // == STDOUT_FILENO

// defs for use in string matching
#define P_STR "|"           // used to string match each input entry
#define QUIT  "quit\n"      // used to string match for closing program

// sig/bool
#define ERR (-1)            // err code
#define FALSE 0
#define TRUE 1

// tags used when outputting messages to stdout
#define DP_TAG  "do_pipe:"
#define IH_TAG  "input_handler:"
#define PO_TAG "pipe_once:"
#define PT_TAG "pipe_twice:"
#define RC_TAG  "run_cmd:"
#define TAG  "myshell: "

// function signatures
char **split_file(char *input);
char *clean_str(char *str);
int elem_count(char **arr);
int found_file(char *file_path);
void connect_pipes(pid_t pid_conn, char **args, int *fst_ends, int *snd_ends);
void do_pipe(char **cmd_args, int size);
void input_handler(char *input);
void pipe_once(char **fst_args, char **snd_args);
void pipe_twice(char **fst_args, char **snd_args, char **thd_args);
void read_second_pipe(pid_t pid_read, char **args, int *fst_ends, int *snd_ends);
void read_single_pipe(pid_t pid_read, char **args, int *pipe_ends);
void run_cmd(char *init_cmd, char *txt_path);
void write_pipe(pid_t pid_write, char **args, int *pipe_ends);


/******************************************************************************
 * Function: main
 * Details: Contains the main loop of the shell program , which will not
 *          exit unless user types 'quit'
 * Input: Command line arguments & argument count
 * Output: Nothing
 ******************************************************************************/
int main(int argc, char **argv) {
	char input[MAX_INP_LEN];
	printf("myshell can run 3 commands: my-cat, my-my-uniq & my-wc."
		   "\nYou may also pipe these commands but the shell will not"
		   " handle more than two pipes.\n");

	int compare_flag = 1; // when this is 0, that means 'quit' has been entered
	while (compare_flag != 0) { // main program/shell loop
		printf("\nmyprompt> ");
		fgets(input, MAX_INP_LEN, stdin);
		compare_flag = strcmp(QUIT, input);
		if (compare_flag == 0) {
			printf("\nSee ya!\n");
			exit(0);
		} else input_handler(input);
	}
}


/******************************************************************************
 * Function: input_handler
 * Details: Reads through the user input and looks for and addresses errors
 *          such as missing files, missing arguments, incorrect input format,
 *          etc. If input and file pass the check then the function decided
 *          whether to run a single command, a command with one pipe or a
 *          command with two pipes.
 * Input: User input (char*)
 * Output: Nothing
 ******************************************************************************/
void input_handler(char *input) {
	if (strcmp(input, "\n") != 0 && (input != NULL)) { // make sure theres some input
		if ((strchr(input, ' ')) == NULL)
			printf("%s%s incorrect arg format or unrecognized command, try again\n", TAG, IH_TAG);

		else {
			char **split_input = split_file(input);
			int arg_cnt = elem_count(split_input);
			char *init_cmd = clean_str(split_input[0]);
			if (found_file(init_cmd) == ERR) printf("\n%s%s oh no can't find file %s\n", TAG, IH_TAG, init_cmd);
			else {

				char *txt_path = clean_str(split_input[1]);

				if (arg_cnt == NO_PIPE) { // basic command
					run_cmd(init_cmd, txt_path);
				} else if (arg_cnt == SINGLE_PIPE || arg_cnt == DOUBLE_PIPE) {
					do_pipe(split_input, arg_cnt);
				} else {
					printf("%s%s Incorrect number of args supplied. Supplied %d args\n", TAG, IH_TAG, arg_cnt);

				}

			}
		}
	}

}


/******************************************************************************
 * Function: run_cmd
 * Details: Runs the simple command supplied at the prompt
 * Input: Command/program name & number of args
 * Output: Nothing
 ******************************************************************************/
void run_cmd(char *init_cmd, char *txt_path) {
	char run[MAX_INP_LEN + 3] = "./";
	strcat(run, init_cmd);
	__pid_t forked = fork();
	if (forked == 0) {
		char *args[] = {run, txt_path, NULL};
		execvp(args[0], args);
		printf("\n");
	} else if (forked < 0) {
		printf("%s%s encountered error while forking", TAG, RC_TAG);
	} else {
		waitpid(forked, 0, 0);
		kill(forked, SIGUSR1);
	}
}


/******************************************************************************
 * Function: do_pipe
 * Details: Checks each input argument and validates accordingly. Based on
 *          number of arguments, this function will either run a single pipe
 *          or double pipe as long as input is formatted correctly
 * Input: A list of arguments (char**) & number of arguments (int)
 * Output: Nothing
 ******************************************************************************/
void do_pipe(char **cmd_args, int size) {

	char *fst_cmd = clean_str(cmd_args[0]);
	char rn_fst[sizeof(fst_cmd) + 3] = "./";
	char *txt_path = clean_str(cmd_args[1]);
	char *pipe_str1 = clean_str(cmd_args[2]);
	char *sec_cmd = clean_str(cmd_args[3]);
	char rn_sec[sizeof(sec_cmd) + 3] = "./";
	strcat(rn_fst, fst_cmd);
	strcat(rn_sec, sec_cmd);

	char *fst_args[] = {rn_fst, txt_path, NULL};
	char *sec_args[] = {rn_sec, NULL};
	if (strcmp(pipe_str1, P_STR) != 0) {
		printf("%s%s Expected pipe but got '%s'.\n", TAG, DP_TAG, pipe_str1);
		return;
	} else if ((found_file(sec_cmd) == ERR)) {
		printf("%s%s Couldn't find '%s'.\n", TAG, DP_TAG, sec_cmd);
		return;
	} else {
		if (size == SINGLE_PIPE) {
			pipe_once(fst_args, sec_args);
		} else if (size == DOUBLE_PIPE) {
			char *pipe_str2 = clean_str(cmd_args[4]);
			char *thd_cmd = clean_str(cmd_args[5]);
			char rn_thd[sizeof(thd_cmd) + 3] = "./";
			strcat(rn_thd, thd_cmd);
			char *thd_args[] = {rn_thd, NULL};

			if (strcmp(pipe_str2, P_STR) != 0) {
				printf("%s%s Expected pipe but got '%s'.\n", TAG, DP_TAG, pipe_str2);
				return;
			} else if (found_file(thd_cmd) == ERR) {
				printf("%s%s Couldn't find '%s'.\n", TAG, DP_TAG, thd_cmd);
				return;
			} else pipe_twice(fst_args, sec_args, thd_args);

		} else {
			printf("%s%s encountered issue.\n", TAG, DP_TAG);
			return;
		}
	}

}


/******************************************************************************
 * Function: pipe_once
 * Details: Handles the execution/implementation for single pipe commands
 * Input: Args for both the first and second commands (char**)
 * Output: Nothing
 ******************************************************************************/
void pipe_once(char **fst_args, char **snd_args) {
	int pipe_ends[2];
	pipe(pipe_ends);
	pid_t writer = -1;
	pid_t reader = -1;
	write_pipe(writer, fst_args, pipe_ends);
	read_single_pipe(reader, snd_args, pipe_ends);
	sleep(2);
	close(pipe_ends[WRITE]);
	close(pipe_ends[READ]);
	wait(NULL);

}


/******************************************************************************
 * Function: pipe_twice
 * Details: Handles the execution/implementation for double pipe commands
 * Input: Args for all three commands (char**)
 * Output: Nothing
 ******************************************************************************/
void pipe_twice(char **fst_args, char **snd_args, char **thd_args) {
	int fst_pipe_ends[2], snd_pipe_ends[2];
	pid_t writer = -1;
	pid_t conn = -1;
	pid_t reader = -1;

	write_pipe(writer, fst_args, fst_pipe_ends);
	close(fst_pipe_ends[WRITE]);

	connect_pipes(conn, snd_args, fst_pipe_ends, snd_pipe_ends);
	close(fst_pipe_ends[READ]);
	close(snd_pipe_ends[WRITE]);

	read_second_pipe(reader, thd_args, fst_pipe_ends, snd_pipe_ends);
	close(snd_pipe_ends[READ]);

	sleep(2);
	wait(NULL);
	wait(NULL);
}


/******************************************************************************
 * Function: write_pipe
 * Details: Used by both pipe_once & pipe_twice to redirect the first commands
 *          output to the write end of the pipe.
 * Input: pid for child process (pid_t), args for the command that is writing to
 *        the pipe (char**) & the file descriptors for the pipe (int*)
 * Output: Nothing
 ******************************************************************************/
void write_pipe(pid_t pid_write, char **args, int *pipe_ends) {
	if (pipe(pipe_ends) == ERR) {
		printf("%s%s error while piping\n", TAG, PO_TAG);
		return;
	}
	pid_write = fork();
	if (pid_write < CHILD) {
		printf("%s%s problem while making fork for writer process\n", TAG, PO_TAG);
		return;
	} else if (pid_write == CHILD) {
		close(pipe_ends[READ]);
		if (dup2(pipe_ends[WRITE], WRITE) == ERR) {
			printf("%s%s dup2 failed on redirecting output\n", TAG, PO_TAG);
			return;
		}
		execv(args[0], args);
	}
}


/******************************************************************************
 * Function: read_single_pipe
 * Details: Used by pipe_once only in order to redirect the commands input
 *          to the read end of the pipe.
 * Input: pid for child process (pid_t), args for the command that is reading
 *        from the pipe (char**) & the file descriptors for the pipe (int*)
 * Output:
 ******************************************************************************/
void read_single_pipe(pid_t pid_read, char **args, int *pipe_ends) {
	pid_read = fork();
	if (pid_read < CHILD) printf("%s%s problem while making fork for reader process\n", TAG, PO_TAG);
	else if (pid_read == CHILD) {
		sleep(1);
		close(pipe_ends[WRITE]); // no need for write end
		if (dup2(pipe_ends[READ], READ) == ERR) { // try to redirect stdin to read end of pipe
			printf("%s%s dup2 failed on redirecting input\n", TAG, PO_TAG);
		}
		execv(args[0], args);
	} else {
		wait(NULL);
	}

}


/******************************************************************************
 * Function: connect_pipes
 * Details: Used by pipe_twice only. Redirects the first commands input to
 *          the read end of the first pipe and redirects the second commands
 *          output to the write end of the second pipe.
 * Input: pid for child process (pid_t), args for the command that is writing to
 *        the pipe (char**) & the file descriptors for both of the pipes (int*)
 * Output: Nothing
 ******************************************************************************/
void connect_pipes(pid_t pid_conn, char **args, int *fst_ends, int *snd_ends) {
	if (pipe(snd_ends) == ERR) {
		printf("%s error while piping\n", TAG);
	}
	pid_conn = fork();
	if (pid_conn < CHILD) {
		printf("%s problem while making fork for writer process\n", TAG);
	} else if (pid_conn == CHILD) {
		sleep(1);
		// try to redirect the input & output of the commands
		if ((dup2(fst_ends[READ], READ) == ERR) || (dup2(snd_ends[WRITE], WRITE) == ERR)) {
			printf("%s error with dup2\n", TAG);
			return;
		}
		close(fst_ends[WRITE]);
		close(fst_ends[READ]);
		close(snd_ends[READ]);
		close(snd_ends[WRITE]);
		execv(args[0], args);
	}

}


/******************************************************************************
 * Function: read_second_pipe
 * Details: Used to read from second pipe only. The main difference between
 *          this function & read_single_pipe is that this function needs to
 *          account for more pipes that it needs to close.
 * Input: pid for child process (pid_t), args for the command that is reading
 *        from the second pipe (char**) & the file descriptors for both of
 *        the pipes (int*)
 * Output: Nothing
 ******************************************************************************/
void read_second_pipe(pid_t pid_read, char **args, int *fst_ends, int *snd_ends) {
	pid_read = fork();
	if (pid_read < CHILD) {
		printf("%s problem while making fork for reader process\n", TAG);
	} else if (pid_read == CHILD) {
		sleep(1);
		// try to redirect stdin to read end of pipe
		if (dup2(snd_ends[READ], READ) == ERR) {
			printf("%s dup2 failed on redirecting input\n", TAG);
		} else {
			// close both pipes
			close(fst_ends[READ]);
			close(fst_ends[WRITE]);
			close(snd_ends[READ]);
			close(snd_ends[WRITE]);
			execv(args[0], args);
		}
	}
}


/******************************************************************************
 * Function: found_file
 * Details: Simple function to check if a file exists
 * Input: File path to be checked (char*)
 * Output: Int value based on whether the file was found or not
 ******************************************************************************/
int found_file(char *file_path) {
	if (access(file_path, R_OK) != ERR) return TRUE;
	else return ERR;
}


/******************************************************************************
 * Function: elem_count
 * Details: Used to get number of elements in a 2D char array
 * Input: Array of strings (char**)
 * Output: Number of strings in array (int)
 ******************************************************************************/
int elem_count(char **arr) {
	int i = 0;
	for (; i < MAX_ARG_LEN + 1; i++) {
		if ((arr[i] == NULL) || strcmp(arr[i], "\n") == 0 || strcmp(arr[i], " ") == 0) break;
	}
	return i;
}


/******************************************************************************
 * Function: split_file
 * Details: Used to split input into string array for easier access to desired
 *          args
 * Input: A single string (char*)
 * Output: An array of strings (char**)
 ******************************************************************************/
char **split_file(char *input) {
	char *split_str = strtok(input, " ");
	char **run_info = malloc(MAX_ARG_LEN * (sizeof(char *)));
	int cmd_count = 0;
	while (split_str != NULL) {
		if (cmd_count > MAX_ARG_LEN) break;
		run_info[cmd_count] = malloc(CMD_LEN * (sizeof(char)));
		run_info[cmd_count] = split_str;
		split_str = strtok(NULL, " ");
		cmd_count++;
	}
	return run_info;
}


/******************************************************************************
 * Function: clean_str
 * Details: Strips all extra whitespace
 * Input: String to be 'cleaned' (char*)
 * Output: The same string with excess whitespace removed
 ******************************************************************************/
char *clean_str(char *str) {
	char *stripped = malloc(strlen(str));
	int new = 0, old = 0;
	for (; old < strlen(str); old++, new++) {
		if (!isspace(str[old])) stripped[new] = str[old];
		else new--;
	}
	stripped[new] = 0;
	return stripped;
}