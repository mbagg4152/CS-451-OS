Maggie Horton
CS-451 Summer 2020
Lab 5

ABOUT
Program takes in at least two integer arguments and then calculates (on separate threads) the values
for the average, maximum, minimum and median then prints the values to stdout.
Note: Median is not calculated in the traditional mathematical sense (based on assumptions made from looking
at example output) in that the values are not sorted. If an odd number of elements are provided then the
median is the value in the middle of the array. If an even number of elements are provided then the median
is the average of the two center values.

COMPILE
Run the command 'make'

RUNNING
There are two different options for running the program
Note: at least 2 integers need to be supplied in the command line arguments.
1.  Regular
    After making, in the terminal run by using './multithread num1 num2 ... num 20'
2.  Using 'make run'
    In the Makefile, add the desired numbers separated by space to variable 'nums'
    In the terminal type 'make run' & hit enter