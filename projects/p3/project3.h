#ifndef PROJECT_3
#define PROJECT_3 1

#define ARR_SZE             300                     // array size
#define BASE                0                       // people start and end at floor 0
#define DEF_FLOORS          10                      // default number of floors
#define DEF_PEOPLE          1                       // default number of people
#define DEF_TIME            10                      // default wandering time
#define D_DOWN              0                       // down represented by 0
#define D_UP                1                       // up represented by 1
#define OPTS                "p:w:f:"                // pattern used in getopt to read cmdline args
#define OPT_F               'f'                     // number of floors option
#define OPT_P               'p'                     // number of people option
#define OPT_W               'w'                     // wander time option
#define PRINT_E             "Elevator:"             // prefix for elevator print statements
#define PRINT_P             "\t\t\t\t\t\tPerson"    // prefix for person print statements
#define WAIT                1                       // elevator wait time
#define true                1
#define false               0
#define PRINT_LEN           2048                    // length of array used in sem protected printing

#include <semaphore.h>


struct Elevator {
    int direction;          // direction of elevator travel
    int next_floor;         // either +1 or -1 of this_floor
    int passed;             // counter for number of floors passed
    int this_floor;         // current floor
};

struct Person {
    int floors[ARR_SZE];    // floors to travel to
    int floors_left;        // remaining floors to visit
    int pid;                // unique personal id
    int this_floor;         // current floor
    int times[ARR_SZE];     // time to be spent on each floor
    int done;
    int wandered[ARR_SZE];
    int last_pair_index;

};

// assign default values to each var in case they're not supplied in args
int body_count = DEF_PEOPLE;    // number of people
int max_time = DEF_TIME;        // max wandering time
int tiers = DEF_FLOORS;         // number of floors in the building


struct Elevator *lift;
struct Person **people;

pthread_t *worker_threads;
pthread_t elev_thread;

sem_t arrival_lock;             // used to start elevator
sem_t display_lock;             // used to surround print statements
sem_t waiting_lock;             // RW protect sem for currently_waiting
sem_t allowed_entry[ARR_SZE];   // RW protect sem for can_enter
sem_t allowed_exit[ARR_SZE];    // RW protect sem for can_exit
sem_t allowed_quit;             // RW protect sem for can_quit
sem_t ready_check;              // RW protect sem for ready_quit_count
sem_t left_check;               // RW protect sem for left_system

int currently_waiting[ARR_SZE]; // holds number of people waiting for the elevator at each floor
int last_person_index = 0;
int top_floor = 0;
int can_exit[ARR_SZE];          // flag for ability to exit at each floor
int can_enter[ARR_SZE];         // flag for ability to enter at each floor
int can_quit;                   // flag for ability to exit the system
int ready_quit_count;           // counter for number of people ready to exit the system
int left_system;                // counts number of people who have left the system

int protected_print(const char *, ...);
void check_stdin();
void get_options(int, char **, int *, int *, int *);
void init_values();
void run_threads();

// elevator functions
void *moving_elevator(void *);
void init_elevator();
void open_doors(int floor);
void show_waiting_people(int);

// people functions
struct Person *init_person(int);
void *riding_elevator(void *);
void enter_lift(struct Person *);
void wander_floor(struct Person *mortal, int time);


#endif