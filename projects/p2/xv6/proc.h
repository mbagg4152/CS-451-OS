
// Per-CPU state
struct cpu {
    uchar apicid;                // Local APIC ID
    struct context *scheduler;   // swtch() here to enter scheduler
    struct taskstate ts;         // Used by x86 to find stack for interrupt
    struct segdesc gdt[NSEGS];   // x86 global descriptor table
    volatile uint started;       // Has the CPU started?
    int ncli;                    // Depth of pushcli nesting.
    int intena;                  // Were interrupts enabled before pushcli?
    struct proc *proc;           // The process running on this cpu or null
};

extern struct cpu cpus[NCPU];
extern int ncpu;

// Saved registers for kernel context switches. Don't need to save all the segment registers (%cs, etc), because they
// are constant across kernel contexts. Don't need to save %eax, %ecx, %edx, because the x86 convention is that the
// caller has saved them. Contexts are stored at the bottom of the stack they describe; the stack pointer is the
// address of the context. The layout of the context matches the layout of the stack in swtch.S at the "Switch
// stacks" comment. Switch doesn't save eip explicitly, but it is on the stack and allocproc() manipulates it.
struct context {
    uint edi; // destination index register
    uint esi; // source index register
    uint ebx; // called base register
    uint ebp; // stack base pointer register
    uint eip; // index pointer register
};
// unused = mark process as unused
// embryo = used to mark process as used and gives unique pid.
// sleeping = mark used before releasing the processor. means process is waiting for an event to complete
// runnable = ready to run but still waiting

enum procstate {
    UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE
};

// Per-process state
struct proc {
    char *kstack;                       // Bottom of kernel stack for this process
    char name[16];                      // Process name (debugging)
    enum procstate state;               // Process state
    int killed;                         // If non-zero, have been killed
    int pid;                            // Process ID
    int priority;                       // Process priority
    pde_t *pgdir;                       // Page table
    struct context *context;            // swtch() here to run process
    struct file *ofile[NOFILE];         // Open files
    struct inode *cwd;                  // Current directory
    struct proc *parent;                // Parent process
    struct trapframe *tf;               // Trap frame for current syscall
    uint sz;                            // Size of process memory (bytes)
    void *chan;                         // If non-zero, sleeping on chan

    // added for Project 2
    int total_ticks[NLAYER];            // Total number of ticks for the process on each layer
    int wait_time[NLAYER];              // Total time the process has waited on each layer
    int tmp_ticks;                      // Ticks counter for proc at current layer. resets on priority change.
    int tmp_wait;                       // Wait counter for proc at current layer. resets on priority change
};

// signatures for functions added for Project 2
int is_waiting(struct proc *);
int procs_exist(int);
void age(int);
void demote(int, int);
void promote(int);


// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap
