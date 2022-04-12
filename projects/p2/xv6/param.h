#define NPROC           64                  // maximum number of processes
#define KSTACKSIZE      4096                // size of per-process kernel stack
#define NCPU            8                   // maximum number of CPUs
#define NOFILE          16                  // open files per process
#define NFILE           100                 // open files per system
#define NINODE          50                  // maximum number of active i-nodes
#define NDEV            10                  // maximum major device number
#define ROOTDEV          1                  // device number of file system root disk
#define MAXARG          32                  // max exec arguments
#define MAXOPBLOCKS     10                  // max # of blocks any FS op writes
#define LOGSIZE         (MAXOPBLOCKS*3)     // max data blocks in on-disk log
#define NBUF            (MAXOPBLOCKS*3)     // size of disk block cache
#define FSSIZE          1000                // size of file system in blocks

// added for Project 2
#define NLAYER          4                   // number of priority queues
#define SLICE_P3        8                   // ticks for P3 priority
#define SLICE_P2        16                  // ticks for second P3 priority
#define SLICE_P1        32                  // ticks for second to P0 priority
#define SLICE_P0        123456789           // filler value, 0 has no time slice
#define P3              3                   // 3rd-level priority val
#define P2              2                   // 2nd-level priority val
#define P1              1                   // 1st-level priority val
#define P0              0                   // 0th-level priority val
#define LIMIT_P3        80                  // max wait time on priority 3, = 10 * slice
#define LIMIT_P2        160                 // max wait time on priority 2, = 10 * slice
#define LIMIT_P1        320                 // max wait time on priority 1, = 10 * slice
#define LIMIT_P0        500                 // max wait time on priority 0