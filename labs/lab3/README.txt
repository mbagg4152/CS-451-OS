Maggie Horton
CS-451 Lab
Lab 3




[NEW FILES]
+ ps.c
+ spin.c


[CHANGES]
sysproc.c
    added to end
        int sys_procStat(void) {
            return procStat();
        }

usys.S
    added to end
        SYSCALL(procStat)

user.h
    added under system calls section
        int procStat(void);

defs.h
    added under proc.c
        int procStat(void);


syscall.c
    added
        extern int sys_procStat(void);
        [SYS_procStat] = sys_procStat

syscall.h
    added
        #define SYS_procStat 22

Makefile
    added to UPROGS=\
        _spin\
        _ps\

    added to EXTRA=\
        spin.c ps.c\

proc.c
    added to end
        // process status
        int procStat() {
        	struct proc *p;
        	// Enable Interrupts on this processor
        	sti();
        	//add the lock functionality
        	//console print
        	cprintf("\n--------------------------------------\n");
        	cprintf("name \t ProcID \t state\n");

        	// loop over the process table looking for process print the states of the process along with their process ID
        	// while they are in SLEEPING, RUNNING, and RUNNABLE states. refer proc.c and scheduler() function

        	acquire(&ptable.lock);
        	for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        		if (p->state == SLEEPING) {
        			cprintf("%s \t %d \t\t sleeping\n", p->name, p->pid);
        		} else if (p->state == RUNNING) {
        			cprintf("%s \t %d \t\t running\n", p->name, p->pid);
        		} else if (p->state == RUNNABLE) {
        			cprintf("%s \t %d \t\t runnable\n", p->name, p->pid);
        		}
        	}
        	cprintf("\n--------------------------------------\n");
        	// release the lock -> refer proc.c and scheduler function
        	release(&ptable.lock);
        	return 22; // system call number (might be different in your case)

        }