#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
#include "pstat.h"
#include <stddef.h>

struct {
    struct spinlock lock;
    struct proc proc[NPROC];
} ptable;

static struct proc *initproc;
int nextpid = 1;
int slices[] = {SLICE_P0, SLICE_P1, SLICE_P2, SLICE_P3};
int limits[] = {LIMIT_P0, LIMIT_P1, LIMIT_P2, LIMIT_P3};

extern void forkret(void);
extern void trapret(void);
static void wakeup1(void *chan);

void pinit(void) {
    initlock(&ptable.lock, "ptable");
}

/* Look in the process table for an UNUSED proc.
 * If found, change state to EMBRYO and initialize state required to run in the kernel.
 * Otherwise return 0. */
static struct proc *allocproc(void) {
    struct proc *p;
    char *sp;
    acquire(&ptable.lock);

    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->state == UNUSED) {

            goto found;
        }
    }
    release(&ptable.lock);
    return 0;

    found:
    p->state = EMBRYO;
    p->pid = nextpid++;
    release(&ptable.lock);

    // Allocate kernel stack.
    if ((p->kstack = kalloc()) == 0) {
        p->state = UNUSED;
        return 0;
    }
    sp = p->kstack + KSTACKSIZE;

    // Leave room for trap frame.
    sp -= sizeof *p->tf;
    p->tf = (struct trapframe *) sp;

    // Set up new context to start executing at forkret, which returns to trapret.
    sp -= 4;
    *(uint *) sp = (uint) trapret;

    sp -= sizeof *p->context;
    p->context = (struct context *) sp;
    memset(p->context, 0, sizeof *p->context);

    p->context->eip = (uint) forkret;

    return p;
}

/* Must be called with interrupts disabled */
int cpuid() {
    return mycpu() - cpus;
}

/* Must be called with interrupts disabled to avoid the caller being rescheduled between
 * reading lapicid and running through the loop. */
struct cpu *mycpu(void) {
    int apicid, i;

    if (readeflags() & FL_IF) {
        panic("mycpu called with interrupts enabled\n");
    }

    apicid = lapicid();
    // APIC IDs are not guaranteed to be contiguous. Maybe we should have a reverse map, or reserve a register to store &cpus[i].
    for (i = 0; i < ncpu; ++i) {
        if (cpus[i].apicid == apicid) {
            return &cpus[i];
        }
    }
    panic("unknown apicid\n");
}

/* Disable interrupts so that we are not rescheduled while reading proc from the cpu structure */
struct proc *myproc(void) {
    struct cpu *c;
    struct proc *p;
    pushcli();
    c = mycpu();
    p = c->proc;
    popcli();
    return p;
}

/*  Set up first user process. */
void userinit(void) {
    struct proc *p;
    extern char _binary_initcode_start[], _binary_initcode_size[];

    p = allocproc();

    initproc = p;
    if ((p->pgdir = setupkvm()) == 0) {
        panic("userinit: out of memory?");
    }
    inituvm(p->pgdir, _binary_initcode_start, (int) _binary_initcode_size);
    p->sz = PGSIZE;
    memset(p->tf, 0, sizeof(*p->tf));
    p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
    p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
    p->tf->es = p->tf->ds;
    p->tf->ss = p->tf->ds;
    p->tf->eflags = FL_IF;
    p->tf->esp = PGSIZE;
    p->tf->eip = 0;  // beginning of initcode.S

    safestrcpy(p->name, "initcode", sizeof(p->name));
    p->cwd = namei("/");
    acquire(&ptable.lock);

    // this assignment to p->state lets other cores run this process. the acquire forces the above writes to be
    // visible, and the lock is also needed because the assignment might not be atomic.
    p->state = RUNNABLE;
    release(&ptable.lock);
}

/* Grow current process's memory by n bytes.
 * return 0 on success, -1 on failure. */
int growproc(int n) {
    uint sz;
    struct proc *curproc = myproc();

    sz = curproc->sz;
    if (n > 0) {
        if ((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0) {
            return -1;
        }
    } else if (n < 0) {
        if ((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0) {
            return -1;
        }
    }
    curproc->sz = sz;
    switchuvm(curproc);
    return 0;
}

/* Create a new process copying p as the parent. Sets up stack to return as if from system call. Caller must set
 * state of returned proc to RUNNABLE. */
int fork(void) {
    int i, pid;
    struct proc *np;
    struct proc *curproc = myproc();

    // Allocate process.
    if ((np = allocproc()) == 0) {
        return -1;
    }

    // Copy process state from proc.
    if ((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0) {
        kfree(np->kstack);
        np->kstack = 0;
        np->state = UNUSED;
        return -1;
    }
    np->sz = curproc->sz;
    np->parent = curproc;
    *np->tf = *curproc->tf;

    // Clear %eax so that fork returns 0 in the child.
    np->tf->eax = 0;

    for (i = 0; i < NOFILE; i++) {
        if (curproc->ofile[i]) {
            np->ofile[i] = filedup(curproc->ofile[i]);
        }
    }

    np->cwd = idup(curproc->cwd);
    safestrcpy(np->name, curproc->name, sizeof(curproc->name));
    pid = np->pid;
    acquire(&ptable.lock);
    np->state = RUNNABLE;
    np->priority = P3;
    release(&ptable.lock);
    return pid;
}

/* Exit the current process.  Does not return. An exited process remains in the zombie state until its parent calls
 * wait() to find out it exited. */
void exit(void) {
    struct proc *curproc = myproc();
    struct proc *p;
    int fd;

    if (curproc == initproc) {
        panic("init exiting");
    }

    // Close all open files.
    for (fd = 0; fd < NOFILE; fd++) {
        if (curproc->ofile[fd]) {
            fileclose(curproc->ofile[fd]);
            curproc->ofile[fd] = 0;
        }
    }

    begin_op();
    iput(curproc->cwd);
    end_op();
    curproc->cwd = 0;

    acquire(&ptable.lock);

    // Parent might be sleeping in wait().
    wakeup1(curproc->parent);

    // Pass abandoned children to init.
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->parent == curproc) {
            p->parent = initproc;
            if (p->state == ZOMBIE) {
                wakeup1(initproc);
            }
        }
    }

    // Jump into the scheduler, never to return.
    curproc->state = ZOMBIE;
    sched();
    panic("zombie exit");
}

/*  Wait for a child process to exit and return its pid.
    Return -1 if this process has no children. */
int wait(void) {
    struct proc *p;
    int havekids, pid;
    struct proc *curproc = myproc();

    acquire(&ptable.lock);
    for (;;) {
        // Scan through table looking for exited children.
        havekids = 0;
        for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
            if (p->parent != curproc) {
                continue;
            }
            havekids = 1;
            if (p->state == ZOMBIE) {
                // Found one.
                pid = p->pid;
                kfree(p->kstack);
                p->kstack = 0;
                freevm(p->pgdir);
                p->pid = 0;
                p->parent = 0;
                p->name[0] = 0;
                p->killed = 0;
                p->state = UNUSED;
                release(&ptable.lock);
                return pid;
            }
        }

        // No point waiting if we don't have any children.
        if (!havekids || curproc->killed) {
            release(&ptable.lock);
            return -1;
        }

        // Wait for children to exit.  (See wakeup1 call in proc_exit.)
        sleep(curproc, &ptable.lock);  //DOC: wait-sleep
    }
}

/*************************************************************************************************************
 * [Modified for Project 2]
 * Per-CPU process scheduler. Each CPU calls scheduler() after setting itself up. Scheduler never returns.
 * It loops, doing: - choose a process to run - swtch to start running that process - eventually that process
 * transfers control via swtch back to the scheduler.
*************************************************************************************************************/
void scheduler(void) {
    struct proc *task;
    struct cpu *c = mycpu();
    c->proc = 0;

    for (;;) {
        sti(); // Enable interrupts on this processor.
        acquire(&ptable.lock);
        int queue;

        for (queue = 3; queue >= 0; queue--) { // since P3 priority is p3 start at last index to first
            for (task = ptable.proc; task < &ptable.proc[NPROC]; task++) {
                int c_pri = task->priority;
                int c_pid = task->pid;
                int c_state = task->state;
                int c_slice = slices[c_pri]; // time slice value based on priority

                // different execution rules for pri 3-1 and pri 0
                if (queue == P3 || queue == P2 || queue == P1) {
                    // check for processes at current priority
                    if (procs_exist(queue)) {
                        // don't do anything if proc is not runnable or if current process does not have
                        // the same value as queue for priority
                        if (c_pri != queue || c_state != RUNNABLE) {
                            continue;
                        }
                        // execute for the allotted time slice
                        for (int i = task->tmp_ticks; i < c_slice; i++) {
                            c->proc = task;
                            age(c_pid); // age the non-running processes
                            switchuvm(task);
                            task->state = RUNNING;
                            swtch(&(c->scheduler), task->context);
                            switchkvm();
                            task->tmp_ticks++; // increment runtime counter
                            task->total_ticks[c_pri]++; // increment run time for this proc on this pri
                            // if there is nothing else to do with process then move on
                            if (task->state != RUNNABLE) { break; }
                        }
                        // if proc still exists and has used up its time slice then demote it to next
                        // priority and move on to look at the next process
                        if (task->tmp_ticks >= c_slice) {
                            demote(c_pid, c_slice);
                            continue;
                        }
                    } else { // nothing to do here
                        continue;
                    }

                } else { // run processes at pri 0 in FIFO fashion
                    while (task->state == RUNNING || task->state == RUNNABLE) {
                        c->proc = task;
                        age(c_pid); // age non-running tasks
                        switchuvm(task);
                        task->state = RUNNING;
                        swtch(&(c->scheduler), task->context);
                        switchkvm();
                        task->tmp_ticks++;  // increment runtime counter
                        task->total_ticks[c_pri]++; // increment run time for this proc on pri 0
                        if (task->state != RUNNABLE) { break; }
                    }
                }
            }
            c->proc = 0;
        }
        release(&ptable.lock);
    }
}

/*************************************************************************************************************
 * [Added for Project 2]
 * increase the waiting time of all process except for the current running one
*************************************************************************************************************/
void age(int pid) {
    struct proc *task;
    for (task = ptable.proc; task < &ptable.proc[NPROC]; task++) {
        if (is_waiting(task) && (task->pid != pid)) { // do not want to age running process
            task->tmp_wait++; // increase current wait time
            task->wait_time[task->priority]++; // increase total wait time on this layer
            if (task->tmp_wait >= limits[task->priority]) {
                promote(task->pid); // try to promote the process
            }
        }
    }
}

/*************************************************************************************************************
 * [Added for Project 2]
 * determines whether or not a process needs a promotion based on its current wait time. if it
 * passes the check, then the program resets the wait time at that level to zero and promotes the
 * process
*************************************************************************************************************/
void promote(int pid) {
    struct proc *task;
    for (task = ptable.proc; task < &ptable.proc[NPROC]; task++) {
        if (task->pid != pid) { // do not want to promote running process
            if (is_waiting(task)) {
                // double-check to ensure process is at or has exceeded wait time
                int wait_lim = limits[task->priority];
                if ((task->priority != P3) && task->tmp_wait >= wait_lim) {
                    task->priority++; // promote priority
                    task->tmp_wait = 0; // reset current wait
                    task->tmp_ticks = 0; // reset current ticks
                }
            }
        }
    }
}

/*************************************************************************************************************
 * [Added for Project 2]
 * Determines whether or not a process needs a demotion based on its current tick count. if it
 * passes the check then current ticks & wait time are cleared as the process will now be shifting priority
*************************************************************************************************************/
void demote(int pid, int c_slice) {
    struct proc *task;
    for (task = ptable.proc; task < &ptable.proc[NPROC]; task++) {
        // double check that proc is at or has exceeded the allotted time slice
        if ((task->pid == pid) && task->tmp_ticks >= c_slice) {
            task->priority--; // decrease priority
            task->tmp_ticks = 0; // reset current ticks
            task->tmp_wait = 0; // reset current wait
        }
    }
}

/*************************************************************************************************************
 * [Added for Project 2]
 * simple function that determines if a process is waiting based on its state.
*************************************************************************************************************/
int is_waiting(struct proc *task) {
    int c_state = task->state;
    return (c_state == RUNNABLE) || (c_state == EMBRYO) || (c_state == SLEEPING);
}

/*************************************************************************************************************
 * [Added for Project 2]
 * Checks to see if there are any valid process that have the priority specified in the parameter.
*************************************************************************************************************/
int procs_exist(int priority) {
    struct proc *task;
    int counter = 0;
    // count number of procs found at given priority level. if 0 then using this in function equates to false
    for (task = ptable.proc; task < &ptable.proc[NPROC]; task++) {
        if (task->priority == priority) {
            counter++;
        }
    }
    return counter;
}

/*************************************************************************************************************
 * [Added for Project 2]
 * System call used to get information about all of the processes.
*************************************************************************************************************/
int getpinfo(struct pstat *stat) {
    // something went wrong, throw error
    if (stat == NULL) {
        return -1;
    }

    struct proc *task;
    acquire(&ptable.lock);
    int i;
    for (i = 0; i < NPROC; i++) {
        task = &ptable.proc[i]; // get current proc for loop

        // set inuse at i based on process
        if (task->state == UNUSED) {
            stat->inuse[i] = 0;
        } else {
            stat->inuse[i] = 1;
        }
        // get pid at i
        stat->pid[i] = task->pid;

        // get ticks for process at i for each priority
        for (int j = 0; j < 4; j++) {
            stat->ticks[i][j] = task->total_ticks[j];
        }

        // get wait time for process at i for each priority
        for (int j = 0; j < 4; j++) {
            stat->wait_ticks[i][j] = task->wait_time[j];
        }
        // get proc priority
        stat->priority[i] = task->priority;

        // get proc state
        stat->state[i] = task->state;
    }
    release(&ptable.lock);
    return 0;
}

/* Enter scheduler.  Must hold only ptable.lock and have changed proc->state. Saves and restores intena because
 * intena is a property of this kernel thread, not this CPU. It should be proc->intena and proc->ncli, but that would
 * break in the few places where a lock is held but there's no process. */
void sched(void) {
    int intena;
    struct proc *p = myproc();

    if (!holding(&ptable.lock)) {
        panic("sched ptable.lock");
    }
    if (mycpu()->ncli != 1) {
        panic("sched locks");
    }
    if (p->state == RUNNING) {
        panic("sched running");
    }
    if (readeflags() & FL_IF) {
        panic("sched interruptible");
    }
    intena = mycpu()->intena;
    swtch(&p->context, mycpu()->scheduler);
    mycpu()->intena = intena;
}

/* Give up the CPU for one scheduling round. */
void yield(void) {
    acquire(&ptable.lock);  //DOC: yieldlock
    myproc()->state = RUNNABLE;
    sched();
    release(&ptable.lock);
}

/* A fork child's very first scheduling by scheduler() will swtch here.
 * "Return" to user space. */
void forkret(void) {
    static int first = 1;
    // Still holding ptable.lock from scheduler.
    release(&ptable.lock);

    if (first) {
        // Some initialization functions must be run in the context of a regular process (e.g., they call
        // sleep), and thus cannot be run from main().
        first = 0;
        iinit(ROOTDEV);
        initlog(ROOTDEV);
    }
    // Return to "caller", actually trapret (see allocproc).
}

/* Atomically release lock and sleep on chan. Reacquires lock when awakened. */
void sleep(void *chan, struct spinlock *lk) {
    struct proc *p = myproc();

    if (p == 0) {
        panic("sleep");
    }

    if (lk == 0) {
        panic("sleep without lk");
    }

    // Must acquire ptable.lock in order to change p->state and then call sched. Once we hold ptable.lock,
    // we can be guaranteed that we won't miss any wakeup (wakeup runs with ptable.lock locked), so it's
    // okay to release lk.
    if (lk != &ptable.lock) {  //DOC: sleeplock0
        acquire(&ptable.lock);  //DOC: sleeplock1
        release(lk);
    }
    // Go to sleep.
    p->chan = chan;
    p->state = SLEEPING;

    sched();

    // Tidy up.
    p->chan = 0;

    // Reacquire original lock.
    if (lk != &ptable.lock) {  //DOC: sleeplock2
        release(&ptable.lock);
        acquire(lk);
    }
}

/* Wake up all processes sleeping on chan. The ptable lock must be held. */
static void wakeup1(void *chan) {
    struct proc *p;

    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
        if (p->state == SLEEPING && p->chan == chan) {
            p->state = RUNNABLE;
        }
}

/* Wake up all processes sleeping on chan. */
void wakeup(void *chan) {
    acquire(&ptable.lock);
    wakeup1(chan);
    release(&ptable.lock);
}

/* Kill the process with the given pid. Process won't exit until it returns to user space (see
 * trap in trap.c). */
int kill(int pid) {
    struct proc *p;

    acquire(&ptable.lock);
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->pid == pid) {
            p->killed = 1;
            // Wake process from sleep if necessary.
            if (p->state == SLEEPING) {
                p->state = RUNNABLE;
            }

            release(&ptable.lock);
            return 0;
        }
    }
    release(&ptable.lock);
    return -1;
}

/* Print a process listing to console.  For debugging. Runs when user types ^P on console. No lock to
 * avoid wedging a stuck machine further. */
void procdump(void) {
    static char *states[] = {
            [UNUSED] = "unused", [EMBRYO] = "embryo", [SLEEPING]  = "sleep ", [RUNNABLE]  = "runnable",
            [RUNNING] = "run", [ZOMBIE] = "zombie"};
    int i;
    struct proc *p;
    char *state;
    uint pc[10];

    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->state == UNUSED) {
            continue;
        }
        if (p->state >= 0 && p->state < NELEM(states) && states[p->state]) {
            state = states[p->state];
        } else {
            state = "???";
        }
        cprintf("%d %s %s", p->pid, state, p->name);
        if (p->state == SLEEPING) {
            getcallerpcs((uint *) p->context->ebp + 2, pc);
            for (i = 0; i < 10 && pc[i] != 0; i++)
                cprintf(" %p", pc[i]);
        }
        cprintf("\n");
    }
}

/*************************************************************************************************************
 * [Leftovers from lab]
 * Used Maggie's code as a starting point so the old sys calls remain.
*************************************************************************************************************/
/* process status */
int procStat() {
    struct proc *p;
    // Enable Interrupts on this processor
    sti();

    //add the lock functionality

    //console print
    cprintf("\n----------------------------------------------\n");
    cprintf("name \t ProcID \t state \t\t priority\n");

    // loop over the process table looking for process print the states of the process along with their process ID
    // while they are in SLEEPING, RUNNING, and RUNNABLE states. refer proc.c and scheduler() function

    acquire(&ptable.lock);
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->state == SLEEPING) {
            cprintf("%s \t %d \t\t sleeping \t %d\n", p->name, p->pid, p->priority);
        } else if (p->state == RUNNING) {
            cprintf("%s \t %d \t\t running \t %d\n", p->name, p->pid, p->priority);
        } else if (p->state == RUNNABLE) {
            cprintf("%s \t %d \t\t runnable \t %d\n", p->name, p->pid, p->priority);
        }
    }
    cprintf("\n----------------------------------------------\n");
    // release the lock -> refer proc.c and scheduler function
    release(&ptable.lock);
    return 22; // system call number (might be different in your case)

}

/* change priority */
int chpr(int pid, int priority) {
    struct proc *p;
    acquire(&ptable.lock);
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->pid == pid) {
            p->priority = priority;
            break;
        }
    }
    release(&ptable.lock);
    return pid;
}
