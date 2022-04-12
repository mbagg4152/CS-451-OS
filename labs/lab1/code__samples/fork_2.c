#include <stdio.h>
#include <sys/types.h>
int main()
{
    fork();
    fork();
    fork();
    printf("hello\n");
    return 0;
}

/*
Number of times hello printed is equal to number of process created. Total Number of Processes = 2n
where n is number of fork system calls. So here n = 3, 23 = 8

Let us put some label names for the three lines:

fork ();   // Line 1
fork ();   // Line 2
fork ();   // Line 3

       L1       // There will be 1 child process
    /     \     // created by line 1.
  L2      L2    // There will be 2 child processes
 /  \    /  \   //  created by line 2
L3  L3  L3  L3  // There will be 4 child processes
                // created by line 3

So there are total eight processes (new child processes and one original process).

*/
