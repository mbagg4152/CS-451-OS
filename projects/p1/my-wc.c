/******************************************************************************
 * Maggie Horton
 * CS-451
 * Summer 2020
 * Project 1, my-wc.c: Takes in a text file or data piped from stdin &
 * 					   performs similarly to the linux command wc.
 * 					   It counts the number of lines & words present in a file.
 ******************************************************************************/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void my_wc(char *path);

#define NO_PATH "#%?no#%?path#%?" // used as a 'flag', very unlikely file name
#define TAG  "my-wc:" // used for printing err messages
int cap = 512; // initial array capacity


/******************************************************************************
 * Function: main
 * Details: This is the main function for the my-wc command which just
 *          checks for a file path in args. If no path is found, then it
 *          assumes that it is being passed data from a pipe, then a flag-string
 *          is passed to wc() so that it knows to look for data from stdin.
 * Input: Command/program name (char*) & input text file path (char*) supplied
 *        from the commandline.
 * Output: 0 upon successful execution
 ******************************************************************************/
int main(int argc, char **argv) {
	char *txt_path;
	if (argc == 1) txt_path = NO_PATH; // tell my_wc to read from stdin
	else txt_path = argv[1]; // pass path of regular file
	my_wc(txt_path);
	return 0;
}


/******************************************************************************
 * Function: wc
 * Details: Replicates the base functionality of the linux command wc. Takes in
 *          a file & outputs the number of lines & words in the file.
 * Input: Path to file (char*)
 * Output: Nothing
 ******************************************************************************/
void my_wc(char *path) {
	char *data;
	int counter = 0;
	int words = 0;
	int lines = 0;
	long txt_size;
	if (strcmp(path, NO_PATH) == 0) {
		int used = 0;
		data = (char *) malloc(sizeof(char) * cap);
		int tmp = getchar();
		while (tmp != EOF) {
			if (used == cap) data = realloc(data, (sizeof(char) * (cap *= 2))); // resize array
			data[used] = (char) tmp;
			used++;
			tmp = getchar();
		}
		txt_size = used;
		if (txt_size == 0) { // required err message & action on file access failure
			printf("%s There was no file name supplied & stdin is empty. Nothing to do here\n", TAG);
			exit(1);
		}
	} else {
		FILE *txt = fopen(path, "r");
		fseek(txt, 0, SEEK_END);
		txt_size = ftell(txt);
		rewind(txt);
		data = malloc(txt_size);
		fread(data, 1, txt_size, txt);
		fclose(txt);
	}

	if (isalnum(data[0])) words++;
	for (; counter < txt_size; counter++) {
		char cur = data[counter];
		if (isspace(cur)) {
			if (counter < txt_size - 1) {
				char next = data[counter + 1];
				if (isalnum(next)) {
					if (cur == '\n') {
						words++;
						lines++;
					} else if ((cur == ' ') || (cur == '\t')) words++;
				} else if (cur == '\n') lines++;
			}
		}

	}

	fprintf(stdout, "\n%s Lines = %d & words = %d\n", TAG, lines, words);

}
