#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

void forkexample()
{
    int pid = fork();

    if (pid<0){
        
        fprintf(stderr, "Fork failed!\n");
        exit(-1);
    }

    // child process because return value zero
    if (pid == 0){

        printf("Process: %d\tHello from Child!\n",getpid());
        //sleep(2);
        //execlp("pwd", "pwd", NULL);
    }

    // parent process because return value non-zero.
    else{
        waitpid(pid, NULL, 0);
        printf("\nProcess: %d\tHello from Parent!\n", getpid());
    }
}
int main()
{
    forkexample();
    return 0;
}


/*

In the above code, a child process is created, fork() returns 0 in the child process and
positive integer to the parent process. Here, two outputs are possible because the parent process
and child process are running concurrently. So we don’t know if OS first give control to which
process a parent process or a child process.

Important: Parent process and child process are running the same program, but it does not mean they are identical.
OS allocate different data and state for these two processes and also control the flow of these processes can be different.
See next example
*/
