Maggie Horton
CS-451 Lab
Lab 4


[Descriptions]
1. Scheduling: 	assign first process high priority. loop through all the processes. if the high priority process
	   		is compared against another process with a lesser priority value, then the new value is the
	   		high priority process. after assignment is done & prioroty is checked, scheduling is the same
	   		as it was before.
2. User command: 	takes in two ints, process id & desired priority. once done, the specified processes priority 
			is updated based on the user input.
3. System call:	takes values from user command and updates the apprioriate process with the specified priority.


[Changes from Lab 4]
new files:		- tester.c
			- chngp.c
in proc.h:		- added int priority
in proc.c:		- added new function chpr()
			- in allocproc(): added p->priority = 10
			- in procStat(): updated print statement to print priority
			- modified scheduler() in order to handle priority
in exec.c:		- added curproc->priority = 3
in Makefile:		- in UPROGS added _tester _chngp
			- in EXTRA added tester.c chngp.c
in sysproc.c:		- new function sys_chpr()
in syscall.c:		- added extern int sys_chpr(void)
                 	- added [SYS_chpr] = sys_chpr
in usys.S:		- added syscall(chpr)
in user.h: 		- added int chpr(int, int)
in defs.h:		- added int chpr(int, int) 
in syscall.h: 		- added #define SYS_chpr 23


[Changes from Lab 3]
new files:		- ps.c
			- spin.c
in sysproc.c: 		- added int sys_procStat(void)
in usys.S: 		- added SYSCALL(procStat)
in user.h: 		- added int procStat(void)
in defs.h: 		- added int procStat(void);
in syscall.c: 		- added extern int sys_procStat(void)
                 	- added [SYS_procStat] = sys_procStat
in syscall.h: 		- added #define SYS_procStat 22
in Makefile: 		- in UPROGS added _spin _ps
    			- in EXTRA added spin.c ps.c
proc.c, added:		- added int procStat()
