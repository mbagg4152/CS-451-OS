/*
    Maggie Horton
    CS-451
    Summer 2020
    myshell.c: takes input to either run mysort.c or quit
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define maxlen 50
#define good 2 // not needed probs, maybe do tru instead
#define bad -1 // not needed probs, maybe do false instead idk also maybe have it be 0 

// these constants arent rly necessary
const char* key = "quit\n";
const char* dnl = "\n\n";
const char* fnl = "\n\n\n\n";

// instead of doing func sigs, probably just place them in order of usage, with main @ the bottom
void runSort(char* cname, char* fname);
int inDir(char* fpath); 

int main(int argc, char** argv){
    
    // could be better to have some of the stuff in the loops in their own functions
    char input[maxlen];
    int comp = 1;  // maybe create a "bool" flag for the while loop instead
    while(comp != 0){
        printf("myprompt> ");
        fgets(input, maxlen, stdin);
        comp = strcmp(key, input);
        if(comp == 0) {
            printf("%sSee ya!%s", fnl, dnl);
            break;
        }
        
        // all the string stuff could probably be moved into different function
        char* space = strchr(input, ' '); // i dont think theres a check for invalid input so this may not be necessary
        if(space == NULL) printf("incorrent arg format, try again\n"); // avoid segfault w/ strtok
        else {
            char* cname = strtok(input, " "); // get name of program to run
            char* fname = strtok(NULL, " "); // get argument for program
            int fnameLen = strlen(fname);
            fname[fnameLen - 1] = '\0'; // just gotta make sure that extra spaces are removed or else it doesnt work right
        
            // maybe instead of tiny little function inDir, just do what the function does here
            if(inDir(cname) == bad) printf("%scan't find specified file %s, try again%s", dnl, cname, dnl);
            else if(inDir(fname) == bad) printf("%scan't find specified file %s, try again%s", dnl, fname, dnl);
            else {
                printf("%s", dnl);
                runSort(cname,fname);  // tbh could probably just move the stuff from runSort here
                printf("%s", dnl);
            }
        }
    }
    return 0;
}

void runSort(char* cname, char* fname){ // create process to run mysort
    __pid_t pid = fork();
    char runArg[maxlen + 3] = "./"; // maybe concat ./ before sending here
    strcat(runArg, cname);

    if (pid == 0){ // the child
        char* args[] = {runArg, fname, NULL};
        execvp(args[0], args);
    }   else waitpid(pid,0,0);
}

int inDir(char* fpath){ // checks if file is present in given path, could potentially use other methods to file check HINT HINT
    if(access(fpath, F_OK) != bad) return good;
    else return bad;
}

