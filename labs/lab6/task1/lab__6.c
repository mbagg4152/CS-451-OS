/*************************************************************************************************************
 * Maggie Horton
 * CS-451L Summer 2020
 * lab__6.c, task 1: added threads to generate numbers and used mutex lock & unlock in the critical section,
 * which was the part of the code modifying the sum variable.
*************************************************************************************************************/

#include <stdlib.h>         // NULL
#include <stdio.h>          // printf
#include <sys/types.h>      // pid_t
#include <unistd.h>         // get_pid
#include <pthread.h>        // threads

#define PRODUCER_NO 5       //Number of producers
#define NUM_PRODUCED 2000   //Number of items to be produced

long sum;                   // Sum of generated values
int finished_producers;     // number of the producer that finished producing

// C: Mutex declaration and initialization
static pthread_mutex_t gen_mutex = PTHREAD_MUTEX_INITIALIZER;

// function signatures
void *generator_function(void *v_param);
void *print_function(void);

int main(void) {
    // initialize random seed
    srand(time(NULL));
    sum = 0;

    // A: Create five generator threads
    pthread_t gen_thread1, gen_thread2, gen_thread3, gen_thread4, gen_thread5;
    pthread_create(&gen_thread1, NULL, generator_function, NULL);
    pthread_create(&gen_thread2, NULL, generator_function, NULL);
    pthread_create(&gen_thread3, NULL, generator_function, NULL);
    pthread_create(&gen_thread4, NULL, generator_function, NULL);
    pthread_create(&gen_thread5, NULL, generator_function, NULL);

    // B: Makes sure that all generator threads has finished before proceeding
    pthread_join(gen_thread1, NULL);
    pthread_join(gen_thread2, NULL);
    pthread_join(gen_thread3, NULL);
    pthread_join(gen_thread4, NULL);
    pthread_join(gen_thread5, NULL);

    print_function();

    return (0);

}

void *generator_function(void *v_param) {
    int counter = 0;
    long this_sum = 0;

    while (counter < 200L) {
        pthread_mutex_lock(&gen_mutex);
        long tmp_num = sum; // start of critical section
        long rand_num = rand() % 10;
        printf("current sum of generated num up to now is %ld, adding %ld to it.\n", tmp_num, rand_num);
        sum = tmp_num + rand_num; // end of critical section
        pthread_mutex_unlock(&gen_mutex);

        counter++;
        this_sum += rand_num;
        usleep(100000);
    }


    // not as critical of a section but sometimes when lock and unlock is not used, the print decoration
    // and/or colors are disturbed
    pthread_mutex_lock(&gen_mutex);
    printf("\033[1;33m"); // yellow
    printf("---+---+----+----------+---------+---+--+---+------+------+---+--+---\n");
    printf("The sum of produced items by thread %ld at the end is: %ld \n", pthread_self(), this_sum);
    printf("---+---+----+----------+---------+---+--+---+------+------+---+--+---\n\n");
    printf("\033[0m");
    pthread_mutex_unlock(&gen_mutex);

    finished_producers++;

    return (0);
}

void *print_function(void) {
    printf("\033[1;34m"); // blue
    printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("The value of counter at the end is: %ld \n", sum);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("\033[0m");
}
