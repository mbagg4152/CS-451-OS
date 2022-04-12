/******************************************************************************
 * Maggie Horton
 * CS-451
 * Summer 2020
 * Project 1, my-cat.c: Takes in a text file or data piped from stdin &
 * 						performs similarly to the linux command cat.
 * 						It takes each char from the file and then displays
 * 						it to stdout.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define NO_PATH "#%?no#%?path#%?" // used as a 'flag', very unlikely file name
#define TAG  "my-cat:" // used when printing certain messages

void my_cat(char *txt_path);


/******************************************************************************
 * Function: main
 * Details: This is the main function for the my-cat command which just
 *          checks for a file path in args. If no path is found, then it
 *          assumes that it is being passed data from a pipe, then a flag-string
 *          is passed to my-cat() so that it knows to look for data from stdin.
 * Input: Command/program name (char*) & input text file path (char*) supplied
 *        from the commandline.
 * Output: 0 upon successful execution
 ******************************************************************************/
int main(int argc, char **argv) {
	char *inp_path;
	if (argc == 1) inp_path = NO_PATH;
	else inp_path = argv[1];
	my_cat(inp_path);
}


/******************************************************************************
 * Function: cat
 * Details: Replicates the base functionality of the linux command cat. Reads
 *          a file & displays its contents in the terminal.
 * Input: Path of file to display (char*)
 * Output: Nothing
 ******************************************************************************/
void my_cat(char *txt_path) {
	if (strcmp(txt_path, NO_PATH) == 0) {
		int tmp_char = fgetc(stdin);
		if (stdin != NULL) {
			while (tmp_char != EOF) {
				putchar(tmp_char);
				tmp_char = fgetc(stdin);
			}
		} else {
			printf("%s There was no file name supplied & stdin is empty. Nothing to do here", TAG);
			exit(1);
		}

	} else {
		FILE *txt_file = fopen(txt_path, "r");
		if (txt_file == NULL) { // required err message & action on file access failure
			printf("%s cannot open file '%s'.\n", TAG, txt_path);
			exit(1);
		}

		char tmp_print;
		while (!feof(txt_file)) {
			fscanf(txt_file, "%c", &tmp_print);
			fputc(tmp_print, stdout);
		}
	}

}