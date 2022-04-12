/******************************************************************************
 * Maggie Horton
 * CS-451 Summer 2020
 * Lab 2, Task 1
 * mysort.c
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LEN 1000

void sort(int nums[], int len);


/******************************************************************************
 * Function: main
 * Details: looks for file path. if file is found, it reads in the file and
 * 			sorts the numbers stored in it. Written w/ assumption that all
 * 			input files are int only.
 * Input: path to input text file (char*)
 * Output: 0 on success
 ******************************************************************************/
int main(int argc, char **argv) {
	if (argc == 1) { // quit if no args supplied
		printf("no file name was supplied in the args.\n");
		return 0;
	}

	int stored = 0; // count number of spaces taken by array
	int temp; // used for scanning
	int cap = 10; // initial array capacity
	int *nums = (int *) malloc(sizeof(int) * cap); // array to hold numbers from file
	FILE *file = fopen(argv[1], "r"); // read number file

	while ((!feof(file)) && (stored < MAX_LEN)) { // read number file and populate dynamically sized array.
		fscanf(file, "%d", &temp);
		// reallocate space for array if array is full
		if (stored == cap) nums = (int *) realloc(nums, sizeof(int) * (cap *= 2));
		nums[stored] = temp; // put number in array
		stored++;
	}

	sort(nums, stored); // sort and print num array
	free(nums);
	return 0;
}


/******************************************************************************
 * Function: sort
 * Details: Takes an array of ints, sorts them then prints the sorted list.
 * 			Implementation of insertion sort.
 * Input: Array of ints to be sorted (int[]) & the length of the array (int)
 * Output: Nothing
 ******************************************************************************/
void sort(int nums[], int len) {
	int this, prev;
	for (int i = 0; i < len; i++) {
		this = nums[i];
		prev = i - 1;

		// check that this is on at least 2nd iteration & only run if previous num is greater than current
		while ((prev >= 0) && (nums[prev] > this)) {
			nums[prev + 1] = nums[prev]; // swap spots
			prev--; // look at number before
		}
		nums[prev + 1] = this;
	}

	for (int i = 0; i < len; i++) {
		printf("%d\n", nums[i]);
	}
}
