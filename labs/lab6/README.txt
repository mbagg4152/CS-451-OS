Maggie Horton
CS-451L Summer 2020
Lab 6

IDEA
This lab code uses threads to generate numbers and then add to a total sum. In order for the sum to match the
threads, a mutex lock is used whenever modifying the sum variable. In the second task, a thread for the print function
is added and pthread_condition is used in order to tell the print thread to wait until all 5 generator threads
have finished.

FILES:
    task1
    |____ lab__6.c: code with changes made for task 1 only
    |____ full-output.txt: result of running program and directing full output to file
    |____ Makefile: makefile for this lab__6.c

    task2
    |____ lab__6.c: code with changes made for both task 1 & task 2
    |____ full-output.txt: result of running program and directing full output to file
    |____ Makefile: makefile for this lab__6.c

COMPILE & RUN:
    1. `cd task1` or `cd task2`
    2. `make`
    3. `make run`


OUTPUT
    TASK 1 CONDENSED OUTPUT - excluding lines printed during generation
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        The sum of produced items by thread 140077472675584 at the end is: 951
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        The sum of produced items by thread 140077558880000 at the end is: 913
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        The sum of produced items by thread 140077550487296 at the end is: 858
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        The sum of produced items by thread 140077567272704 at the end is: 926
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        The sum of produced items by thread 140077464282880 at the end is: 937
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        The value of counter at the end is: 4585
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    NOTE: Final value is actual sum from all threads

    TASK 2 CONDENSED OUTPUT - excluding lines printed during generation
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        The sum of produced items by thread 140527460157184 at the end is: 868
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        The sum of produced items by thread 140527468549888 at the end is: 917
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        The sum of produced items by thread 140527493728000 at the end is: 963
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        The sum of produced items by thread 140527485335296 at the end is: 899
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        The sum of produced items by thread 140527476942592 at the end is: 915
        ---+---+----+----------+---------+---+--+---+------+------+---+--+---
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        The value of counter at the end is: 4562
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    NOTE: Final value is actual sum from all threads

