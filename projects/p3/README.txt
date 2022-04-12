Brendan Gallagher
Maggie Horton
CS-451 Summer 2020
Project 3

The input file that was used and the according output is included in this dir as input.txt and output.txt

ABOUT
This project uses semaphores in order to implement synchronization by simulating people riding on an elevator.
There is one elevator thread and N people threads. Most of the synchronization logic is applied by creating
semaphore-protected global variables.

Note: If for some reason a wait time or floor number from the input file is higher than the supplied values,
then the values will be set to the supplied values instead.

COMPILE
In the same dir as the program and inout file, type `make` into terminal.

RUN
1. Via editing Makefile
    1. modify the args variable such that args=-p 'no_of_people' -w 'max_wait' -f 'floor_count' < 'input_file_name'
    2. type `make run` into terminal
2. Via terminal
    1. in the terminal type `./project3 -p 'no_of_people' -w 'max_wait' -f 'floor_count' < 'input_file_name'`

CLEAN
In the same dir as the program and inout file, type `make clean` into terminal.
