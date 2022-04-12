#include <bits/types/FILE.h>
#define BS_NAME             "BACKING_STORE.bin" // name of backing store file
#define BYTE                8                   // used in bit shifting
#define FRAMES              256                 // number of frames
#define FRAME_INIT          -1                  // init value for frames
#define MASK_ADDR           0b1111111111111111  // address mask
#define MASK_OFFSET         0b11111111          // offset mask
#define MODE_STORE          "rb"                // mode for opening backing store file
#define MODE_TXT            "r"                 // mode for opening address text file
#define SZ_BUFF             16                  // TLB size
#define SZ_FRAME            256                 // frame size
#define SZ_LINE             10                  // size of each address line
#define SZ_STORE            256                 // backing store size
#define SZ_TABLE            256                 // page table size

FILE *addr_file;                                // file containing addresses
FILE *store_file;                               // backing store file
char input_addr[SZ_LINE];                       // used to hold one address value at a time
float fault_rate;                               // rate of page faults
float hit_rate;                                 // TLB hit rate
int addr_physical;                              // physical address
int addr_virtual;                               // virtual address
int avail_entry_index = 0;                      // keep track of first free table entry
int avail_frame_index = 0;                      // keep track of first free frame
int buff_entries = 0;                           // counter for TLB entries
int buff_frames[SZ_BUFF];                       // hold TLB frame numbers
int buff_hits = 0;                              // keep track of TLB hits
int buff_pages[SZ_BUFF];                        // hold TLB page numbers
int faults = 0;                                 // page fault counter
int physical_mem[FRAMES][SZ_FRAME];             // physical memory
int table_frames[SZ_TABLE];                     // hold page table frame numbers
int table_pages[SZ_TABLE];                      // hold page table page numbers
int trans_addr = 0;                             // counter for translated addresses
signed char bs_data[SZ_STORE];                  // hold data from backing store
signed char this_val;                           // used to store current value in memory

int get_page(int, int);
int get_physical(int, int);
void buff_assign_end(int, int);
void buff_assign_prev(int, int);
void buff_insert(int, int);
void buff_right_shift(int);
void check_and_open(int, char **);
void get_physical_value(int);
void read_store(int);
void seek_read_check(int);
