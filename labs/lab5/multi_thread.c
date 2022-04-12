/*
 	Maggie Horton
 	CS-451 Lab
 	Summer 2020
 	Lab 5
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define MAX_NUMS 20

// globals
int *ve_nums; // will hold ints supplied as arguments
int total_nums, med, max, min, avg;

// function signatures
int *nums_from_args(int arg_count, char **args);
void *find_average(void *void_nums);
void *find_maximum(void *void_nums);
void *find_minimum(void *void_nums);
void *find_median(void *void_nums);
void run_threads(int *nums);

/*
 	Main function of the program. If the arg count is less than 3 then the program
 	exits. Calls function that runs the threads then prints the values
 	that were updated by function calling in the threads.
 */
int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Need at least 2 numbers supplied in cmdline\n");
		exit(0);
	} else {
		ve_nums = nums_from_args(argc, argv);
		total_nums = argc - 1;
		run_threads(ve_nums);
		printf("The average is %d\nThe minimum is %d\nThe maximum is %d\nThe median is %d\n", avg, min, max, med);
	}
	return 0;
}

/*
 	Makes array of the ints which were supplied as command line args.
 */
int *nums_from_args(int arg_count, char **args) {
	int *nums = malloc(sizeof(int) * MAX_NUMS);

	// need to have an offset since args[0] is not an int arg
	for (int old_index = 1, new_index = 0; old_index < arg_count; old_index++, new_index++) {
		char *tmp = args[old_index];
		if (strlen(tmp) == 0) {
			break;
		} else {
			int tmp_int = atoi(tmp);
			nums[new_index] = tmp_int;
		}
	}
	return nums;
}

/*
 	Takes in the int arguments. Then it makes a thread for each
 	of the functions that calculate the average, maximum, minimum
 	and median and makes sure to sends the numbers to each function.
 	Also makes sure that all threads are done before exiting.
 */
void run_threads(int *nums) {
	pthread_t avg_thread, max_thread, min_thread, med_thread;
	pthread_create(&avg_thread, NULL, find_average, nums);
	pthread_create(&max_thread, NULL, find_maximum, nums);
	pthread_create(&min_thread, NULL, find_minimum, nums);
	pthread_create(&med_thread, NULL, find_median, nums);
	pthread_join(avg_thread, NULL);
	pthread_join(max_thread, NULL);
	pthread_join(min_thread, NULL);
	pthread_join(med_thread, NULL);
}

/*
	Calculates the average of the number supplied and
 	updates the global variable for average.
 */
void *find_average(void *void_nums) {
	int total = 0, count = 0;
	int *nums = (int *) void_nums;
	for (int i = 0; i < total_nums; i++) {
		int tmp = nums[i];
		if (tmp) {
			count++;
			total += tmp;
		}
	}
	avg = total / count;
	return 0;
}

/*
	Finds the maximum value of the numbers supplied
 	and updates the global variable.
 */
void *find_maximum(void *void_nums) {
	max = 0;
	int *nums = (int *) void_nums;
	for (int i = 0; i < total_nums; i++) {
		int tmp = nums[i];
		if (max < tmp) { max = tmp; }
	}
	return 0;
}

/*
	Finds the minimum value of the numbers supplied
 	and updates the global variable.
 */
void *find_minimum(void *void_nums) {
	int *nums = (int *) void_nums;
	min = nums[0];
	for (int i = 1; i < total_nums; i++) {
		int tmp = nums[i];
		if (min > tmp) { min = tmp; }
	}
	return 0;
}

/*
	Finds the median value of the numbers supplied
 	and updates the global variable.
 	The median is not based on sorting the list
 	of ints.
 */
void *find_median(void *void_nums) {
	int *nums = (int *) void_nums;
	int count = 1;
	for (int i = 1; i < total_nums; i++) count++;

	// if even number of ints, then calculate median as the average
	// of the two middle values
	if (count % 2 == 0) {
		int half_count = count / 2;
		int f_num = nums[half_count - 1], s_num = nums[half_count];
		med = (f_num + s_num) / 2;
	}
		// number of ints supplied is odd so median is just the middle value
	else {
		int mid_index = count / 2;
		med = nums[mid_index];
	}
	return 0;
}