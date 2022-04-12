Brendan Gallagher
Maggie Horton
CS-451 Summer 2020
Project 2

COMPILE & RUN
In the terminal type 'make clean; make; make qemu-nox'


MAJOR IDEA
The basis of this project was to implement an MLFQ scheduler to replace the RR scheduler and to add a system call that 
could be used in testing to see if the scheduler works appropriately. The scheduler was designed in attempt to adhere to the following:

1. New processes start at the highest priority

2. PR3 has a time slice of 8, PR2 has a slice of 16 and PR1 has a slice of 32. If process exceeds this time then it is demoted.

3. Procs with PR0 shouldn't wait longer than 500 ticks, procs with PR1 shouldn't wait longer than 320 ticks & procs with PR2 
shouldn't wait longer than 160. If this happens then the process should be promoted. 

4. Procs with PR0 run FIFO, so each process runs until completion, unless it waits long enough such that it becomes promoted.






NOTABLE CHANGES
1. Adding system call: 
	the appropriate changes were made in order to add the system call getpinfo

2. In proc.c:	
	- Modified scheduler in order for it to use MLFQ instead of Round-Robin
	- added 6 new functions:
		- void age(int): 
			takes in the pid of the running processes and for each process not currently running, their current and 
			layer wait time is incremented.
						
		- void promote(int): 
			takes in the pid of the running process and for each waiting process check if it is available for promotion. 
			if it is, then priority is increased and the current tick & wait counters are reset.
						
		- void demote(int, int):
			takes in the pid of the running process and double checks to confirm that it has been running long enough on the 
			current layer to be demoted. if demoted, priority is decremented and current wait and tick counters are reset.
						 
		- int is_waiting(struct proc*):
			simple function that returns true/1 if process is in one of the following states: RUNNABLE, EMBRYO or SLEEPING.
		
		- int procs_exist(int):
			simple function that takes in a priority value and returns the count of processes that were found with said
			priority.
		
		- int getpinfo(struct pstat*):
			system call that takes in a new pstat. the function looks through all of the processes and then fills the
			pstat struct accordingly. returns 0 on success or -1 on failure

3. In proc.h:
	- Added properties to proc struct:
		total_ticks[NLAYER] = time process was running on each priority
		wait_time[NLAYER] = time process was waiting on each priority
		tmp_ticks = time process has been running since creation or priority change
		tmp_wait = time process has been waiting since creation or priority change
		
	- Added function signatures:
		int is_waiting(struct proc *);
		int procs_exist(int);
		void age(int);
		void demote(int, int);
		void promote(int);

4. Added pstat.h and test.c to xv6 directory. The Makefile was updated to handle test.c as a user program.



NOTE ON RUNNING/OUTPUT
When tester is run immediately after xv6 has been started, the scheduler (based on tester's output) appears to be behaving as expected. 
If tester is called again in the same session, it appears that the total ticks per layer had persisted and had been incremented by running
the process again. Not sure if this expected or not.


SAMPLE OUTPUT* ON MACHINE**
$ test 10
Total time used at P3 = 7 ticks
Total time used at P2 = 0 ticks
Total time used at P1 = 0 ticks
Total time used at P0 = 0 ticks

$ test 100000000
Total time used at P3 = 8 ticks
Total time used at P2 = 16 ticks
Total time used at P1 = 32 ticks
Total time used at P0 = 531 ticks


*Output was from two different boot sessions.
**Machine details: ASROCK MoBo running Ubuntu 20 w/ 16 GB RAM







		
	

