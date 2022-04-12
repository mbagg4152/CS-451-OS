Brendan Gallagher
Maggie Horton
Project 4
CS-451 Summer 2020

ABOUT
The program reads in a list of addresses one at a time, given that a file name has been supplied and the address file
and backing store can be accessed. Then try to access value for frame from page table and increase TLB hit counter
or page fault counter accordingly. Get the value for the frame, physical address and the data stored in the physical
address. Then write to TLB.

FILES
project4.c - project 4 code
project4.h - header file for project 4 code
out.txt - output from running code
addresses.txt - input file of addresses

COMPILE
Type `make` into terminal

RUN
Makefile is already set up to use `addresses.txt` as a command line argument for the program. If the address file name
is different make sure to change it in the Makefile.
Type `make run` into the terminal