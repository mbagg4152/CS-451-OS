#include "project3.h"
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <getopt.h>
#include <zconf.h>
#include <fcntl.h>
int main(int argc, char *argv[]) {
    check_stdin();
    get_options(argc, argv, &body_count, &max_time, &tiers);
    init_values();

    run_threads();
    return 0;
}

/*
 * Makes sure that there is data in stdin before creating each person. If stdin is empty, then the function
 * exits the program.
 */
void check_stdin() {
    if (fseek(stdin, 0, SEEK_END), ftell(stdin) > 0) {
        rewind(stdin);
    } else {
        printf("Either a file was not directed or the file supplied is empty. Exiting.\n");
        exit(1);
    }
}

/*
 * Parse command line arguments to get the number of people, max wait time and floor count.
 */
void get_options(int a_count, char **o_args, int *p_count, int *w_time, int *f_count) {
    int arg;
    while ((arg = getopt(a_count, o_args, OPTS)) != -1) {
        switch (arg) {
            case OPT_P:
                *p_count = atoi(optarg);
                break;
            case OPT_W:
                *w_time = atoi(optarg);
                break;
            case OPT_F:
                *f_count = atoi(optarg);
                break;
            default:
                printf("Something is wrong with the supplied options\n");
                break;
        }
    }
}

/*
 * Initialize important values, mostly semaphores and global variables.
 */
void init_values() {
    can_quit = false;
    last_person_index = body_count - 1;
    left_system = 0;
    ready_quit_count = 0;
    top_floor = tiers - 1;

    for (int i = 0; i < tiers; i++) {
        can_exit[i] = false;
        can_enter[i] = false;
    }

    sem_init(&allowed_quit, 0, 1);
    sem_init(&arrival_lock, 0, 1);
    sem_init(&display_lock, 0, 1);
    sem_init(&left_check, 0, 1);
    sem_init(&ready_check, 0, 1);
    sem_init(&waiting_lock, 0, 1);

    for (int i = 0; i < tiers; i++) {
        sem_init(&allowed_entry[i], 0, 1);
        sem_init(&allowed_exit[i], 0, 1);
    }

    init_elevator();
    people = (struct Person **) malloc(sizeof(struct Person) * (last_person_index));
    worker_threads = (pthread_t *) malloc(sizeof(pthread_t) * (last_person_index));

    for (int i = 0; i < body_count; i++) {
        people[i] = (struct Person *) malloc(sizeof(struct Person));
        people[i] = init_person(i);
    }
}

/*
 * Executes the elevator and people threads.
 */
void run_threads() {
    sem_wait(&arrival_lock);

    for (int i = 0; i < body_count; i++) {
        pthread_create(&worker_threads[i], NULL, riding_elevator, (void *) people[i]);
    }

    sem_post(&arrival_lock);
    pthread_create(&elev_thread, NULL, moving_elevator, (void *) lift);

    for (int i = 0; i < body_count - 1; i++) {
        pthread_join(worker_threads[i], NULL);
    }
    pthread_join(elev_thread, NULL);
    exit(0);
}

/*
 * Set elevator values
 */
void init_elevator() {
    lift = (struct Elevator *) malloc(sizeof(struct Elevator));
    lift->direction = D_UP;
    lift->next_floor = 1;
    lift->this_floor = BASE;
}

/*
 * Printing function used in leu of surrounding each print statement with sem_wait & sem_lock.
 */
int protected_print(const char *message, ...) {
    sem_wait(&display_lock);
    char print_string[PRINT_LEN];
    va_list p_args;
    va_start(p_args, message);
    vsnprintf(print_string, PRINT_LEN, message, p_args);
    puts(print_string);
    va_end(p_args);
    sem_post(&display_lock);
    return 0;
}

/*
 * Main logic for simulating the movement of an elevator. The elevator goes all the way to the top and all
 * the way to the bottom until all of the people have left.
 */
void *moving_elevator(void *args) {
    sem_wait(&arrival_lock); // wait for all people to arrive
    struct Elevator *hoist = (struct Elevator *) args;
    // int vals used to store temporary values from sem-protected globals
    int people_waiting_on_floor, people_ready_to_quit, finished;

    while (1) {
        sem_wait(&left_check);
        finished = left_system;
        sem_post(&left_check);

        if (finished == body_count) {
            protected_print("%s \"All patrons have left the system and so shall I\"\n", PRINT_E);
            pthread_exit(0);
        }

        sem_wait(&waiting_lock);
        people_waiting_on_floor = currently_waiting[hoist->this_floor];
        sem_post(&waiting_lock);

        sem_wait(&ready_check);
        people_ready_to_quit = ready_quit_count;
        sem_post(&ready_check);

        // open doors if people are waiting or are ready to exit the system
        if ((people_waiting_on_floor) || (people_ready_to_quit && hoist->this_floor == BASE)) {
            hoist->passed = 0;
            open_doors(hoist->this_floor);

        } else {
            hoist->passed++;
        }
        if (hoist->this_floor == BASE) { // if on bottom start going up
            if (hoist->direction == D_DOWN) {
                show_waiting_people(D_UP);
                hoist->direction = D_UP;
            }
            hoist->next_floor++;
        } else if (hoist->this_floor == top_floor) { // if on top start going down
            if (hoist->direction == D_UP) {
                show_waiting_people(D_DOWN);
                hoist->direction = D_DOWN;
            }
            hoist->next_floor--;
        } else { // for all other floors, increment or decrement next floor depending on direction
            if (hoist->direction == D_UP) {
                hoist->next_floor++;
            } else if (hoist->direction == D_DOWN) {
                hoist->next_floor--;
            }
        }

        hoist->this_floor = hoist->next_floor;
        sleep(1);
    }
}

/*
 * Simulate opening the elevator doors by allowing people to enter or leave on the current level and then
 * revoking said privileges after waiting for 1 second.
 */
void open_doors(int floor) {

    // allow people to exit and enter
    sem_wait(&allowed_exit[floor]);
    can_exit[floor] = true;
    sem_post(&allowed_exit[floor]);

    sem_wait(&allowed_entry[floor]);
    can_enter[floor] = true;
    sem_post(&allowed_entry[floor]);
    if (floor == BASE) {
        sem_wait(&allowed_quit);
        can_quit = true;
        sem_post(&allowed_quit);
    }

    protected_print("%s Opening on floor %i\n", PRINT_E, floor);
    // wait for passengers
    sleep(WAIT);

    // 'close' the doors and keep moving
    sem_wait(&allowed_exit[floor]);
    can_exit[floor] = false;
    sem_post(&allowed_exit[floor]);

    sem_wait(&allowed_entry[floor]);
    can_enter[floor] = false;
    sem_post(&allowed_entry[floor]);

    sem_wait(&allowed_quit);
    can_quit = false;
    sem_post(&allowed_quit);
}

/*
 * Displays a list of people waiting for the elevator on each level
 */
void show_waiting_people(int direction) {
    // will hold the final output string the number of people waiting on each level is shown
    char *message = NULL;
    size_t msg_size = 0;
    // used to 'concatenate' values to a string
    FILE *print_stream = open_memstream(&message, &msg_size);

    if (direction == D_UP) {
        fprintf(print_stream, "\n%s Going Up!\n", PRINT_E);
    } else if (direction == D_DOWN) {
        fprintf(print_stream, "\n%s Heading Down!\n", PRINT_E);
    }
    fprintf(print_stream, "Number of people (P) waiting on each floor (F)\n");
    int format_counter = 0;
    int format_limit = 3;
    for (int i = 0; i < tiers; i++) {
        fprintf(print_stream, "[F%i P%i] ", i, currently_waiting[i]);
        if (format_counter == format_limit) {
            fprintf(print_stream, "\n");
            format_counter = 0;
        } else {
            format_counter++;
        }

    }
    fprintf(print_stream, "\n\n");
    fclose(print_stream);
    protected_print("%s", message);
    free(message);
}

/*
 * Create each person in the people array based off of the data read from stdin.
 */
struct Person *init_person(int new_pid) {
    struct Person *person = (struct Person *) malloc(sizeof(struct Person));
    // will hold the final output string after each person is made
    char *message = NULL;
    size_t msg_size = 0;
    // used to 'concatenate' values to a string
    FILE *print_stream = open_memstream(&message, &msg_size);
    int wandering_pairs;
    scanf("%i", &wandering_pairs); // read in line of single int from stdin
    person->pid = new_pid;
    person->floors_left = wandering_pairs;
    person->done = 0;
    person->last_pair_index = wandering_pairs - 1;

    for (int i = 0; i < wandering_pairs; i++) {
        scanf("%i", &person->floors[i]);
        scanf("%i", &person->times[i]);
        if (person->times[i] > max_time) {
            person->times[i] = max_time;
        }
        if (person->floors[i] > top_floor) {
            person->floors[i] = top_floor;
        }
        person->wandered[i] = 0;
        fprintf(print_stream, "Person %d: will wander for %i sec. on floor %i\n",
                person->pid, person->times[i], person->floors[i]);
    }
    person->this_floor = person->floors[0];
    if (person->this_floor > top_floor) {
        person->this_floor = top_floor;
    }
    fclose(print_stream);
    protected_print("%s", message);
    free(message);
    return person;

}

/*
 * Main logic for simulating a person riding in an elevator.
 */
void *riding_elevator(void *void_param) {
    struct Person *human = (struct Person *) void_param;

    int index = 0, next_floor = 0, current_wander = 0, wandered_floor = 0;
    while (1) {

        current_wander = human->times[index];


        if (index < human->last_pair_index) {
            next_floor = human->floors[index + 1];
        } else {
            next_floor = human->floors[index];
        }


        if (human->floors_left < 1) {
            human->done = 1;
            int ready;
            sem_wait(&ready_check);
            ready_quit_count++;
            sem_post(&ready_check);

            sem_wait(&allowed_quit);
            ready = can_quit;
            sem_post(&allowed_quit);

            protected_print("%s %d: Ready to leave the system ", PRINT_P, human->pid);
            // cannot leave the elevator until the elevator reaches the ground
            if (!ready) {
                for (;;) {
                    sleep(1);
                    sem_wait(&ready_check);
                    ready = can_quit;
                    sem_post(&ready_check);
                    if (ready)break;
                }
            }
            sem_wait(&ready_check);
            ready_quit_count--;
            sem_post(&ready_check);

            sem_wait(&left_check);
            left_system++;
            sem_post(&left_check);

            protected_print("%s %d: \"I'm outta here\"", PRINT_P, human->pid);
            pthread_exit(0);

        }

        wander_floor(human, current_wander);
        wandered_floor = human->this_floor;

        if (human->wandered[wandered_floor]) {

            enter_lift(human);
        }
        human->this_floor = next_floor;
        index++;
        human->floors_left--;
    }

}

/*
 * Simulate person leaving the elevator and wandering on the floor.
 */
void wander_floor(struct Person *mortal, int time) {
    if (!mortal->done) {
        sem_wait(&waiting_lock);
        currently_waiting[mortal->this_floor]++;
        sem_post(&waiting_lock);

        int ready;
        int floor = mortal->this_floor;

        sem_wait(&allowed_exit[floor]);
        ready = can_exit[floor];
        sem_post(&allowed_exit[floor]);
        protected_print("%s %d: Taking elevator to floor %d", PRINT_P, mortal->pid, floor);
        // cannot leave until elevator is on the same floor
        if (!ready) {
            for (;;) {
                sleep(1);
                sem_wait(&allowed_exit[floor]);
                ready = can_exit[floor];
                sem_post(&allowed_exit[floor]);
                if (ready)break;
            }
        }


        protected_print("%s %d: Wandering on floor %i for %i sec.", PRINT_P,
                        mortal->pid, mortal->this_floor, time);
        sleep(time);
        mortal->wandered[mortal->this_floor] = 1;


        sem_wait(&waiting_lock);
        currently_waiting[mortal->this_floor]--;
        sem_post(&waiting_lock);
    }
}

/*
 * Simulate person waiting for the elevator to pick them up when they are done wandering.
 */
void enter_lift(struct Person *mortal) {
    if (!mortal->done) {

        sem_wait(&waiting_lock);
        currently_waiting[mortal->this_floor]++;
        sem_post(&waiting_lock);

        int ready;
        int floor = mortal->this_floor;

        sem_wait(&allowed_entry[floor]);
        ready = can_enter[floor];
        sem_post(&allowed_entry[floor]);

        if (!ready) {
            for (;;) {
                sleep(1);
                sem_wait(&allowed_entry[floor]);
                ready = can_enter[floor];
                sem_post(&allowed_entry[floor]);
                if (ready)break;
            }
        }

        protected_print("%s %d: Entering elevator on floor %d", PRINT_P, mortal->pid, mortal->this_floor);


        sem_wait(&waiting_lock);
        currently_waiting[mortal->this_floor]--;
        sem_post(&waiting_lock);
    }
}