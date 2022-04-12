/*
    Maggie Horton
    CS-451
    Summer 2020
    mysort.c: takes in number file name in args, reads file to array, 
    sorts then outputs the sorted data.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sort(int nums[], int len); // remove this n then move main to the end or smth

int main(int argc, char** argv){    
    if (argc == 1) { // quit if no args supplied
        printf("no file name was supplied in the args.\n");
        return 0;
    }

    int stored, temp; // dont need stored, prob keep temp but rename 
    int cap = 10; // intial array capacity
    int max = 1000; // max number of vals to read in
    int* nums = (int*) malloc(sizeof(int)*cap); // array to hold numbers from file
    FILE* file = fopen(argv[1], "r"); // read number file 

    
    // dont need to do this fancy realloc shit, max num of nums we need to read in is 1000 so just make beeg array
    while ((!feof(file)) && (stored < max)){ // read number file and populate dynamically sized array.
        fscanf(file, "%d", &temp);
        if (stored == cap) { // change size of array 
            cap = cap *2; // no
            nums = (int*) realloc(nums, sizeof(int)*(cap)); // no
        }
        nums[stored] = temp; // put number in array
        stored++; // probably dont need this bc tfw no realloc
    }
    
    printf("the numbers from the file, sorted:\n\n");
    sort(nums, stored); // sort and print num array
    free(nums); // ig this could stay but w/e its the end of the stuff
    return 0;
}

// use a different sorting algo pls n thank, there r many, hard 2 go wrong
void sort(int nums[], int len){ // implementation of insertion sort
    int this, prev;
    for (int i = 0; i < len; i++){
        this = nums[i];
        prev = i - 1;

        // check that this is on at least 2nd iteration & only run if previous num is greather than current
        while ((prev >= 0) && (nums[prev] > this)){ 
            nums[prev + 1] = nums [prev]; // swap spots
            prev--; // look at number before
        }
        nums[prev + 1] = this;
    }

    // just print the binches 1 per line, also if not using cool array then make sure to do a NULL check or smth so you dont make the computer stroke out
    int fcounter = 0; // counter stored for output formatting    
    for (int i = 0; i < len; i++){
        if (i == (len - 1)) printf("%d\n", nums[i]);
        else if (fcounter == 19){
            printf("%d,\n", nums[i]);
            fcounter = 0;
        }
        else printf("%d, ", nums[i]);        
        fcounter++;
    }
}
