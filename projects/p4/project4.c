#include "project4.h"
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>

int main(int argc, char **argv) {
    check_and_open(argc, argv); // validate args and files
    printf(" Virtual address \t Physical address \t Value \n");
    while (fgets(input_addr, SZ_LINE, addr_file) != NULL) { // parse until no more addresses are left
        addr_virtual = atoi(input_addr); // current virtual memory address
        get_physical_value(addr_virtual); // get and print values for virtual & physical address and mem value
        trans_addr++;
    }

    hit_rate = (float) buff_hits / (float) trans_addr; // TLB hit rate = TLB hits per address translated
    fault_rate = (float) faults / (float) trans_addr; // page fault rate = page faults per address translated

    printf("\n\n %d Translations\n %d Page faults\n %d TLB hits\n %.4f%% Fault rate\n %.4f%% TLB hit rate\n",
           trans_addr, faults, buff_hits, fault_rate, hit_rate);

    // close address text file & backing store
    fclose(addr_file);
    fclose(store_file);
    return EXIT_SUCCESS;
}

/*
 * Print error message and then exit with failure, if applicable. If no error, then both files are opened.
 */
void check_and_open(int argc, char **argv) {
    if (argc < 2) {
        printf("Error: No address file name supplied.\n");
        exit(EXIT_FAILURE);
    }
    if (argc > 2) {
        printf("Error: Only the address file name should be supplied.\n");
        exit(EXIT_FAILURE);
    }
    if ((store_file = fopen(BS_NAME, MODE_STORE)) == NULL) {
        printf("Error: Problem while opening backing store.\n");
        exit(EXIT_FAILURE);
    }
    if ((addr_file = fopen(argv[1], MODE_TXT)) == NULL) {
        printf("Error: Problem while opening address file.\n");
        exit(EXIT_FAILURE);
    }
}

/*
 * Takes in virtual address. Gets physical address and value stored at the physical location, then prints
 * the virtual address, physical address and value.
 */
void get_physical_value(int addr) {
    int frame = FRAME_INIT;
    int page = get_page(addr, MASK_ADDR);
    int offset = (addr & MASK_OFFSET);

    for (int i = 0; i < SZ_BUFF; i++) { // extract frame if TLB index == page then increment TLB hit
        if (buff_pages[i] == page) {
            frame = buff_frames[i];
            buff_hits++;
        }
    }

    // frame number not found. if matching page is found, then extract frame, otherwise page fault
    // occurred --> increment page fault counter and read from backing store to assign frame value to first
    // available frame.
    if (frame == FRAME_INIT) {
        for (int i = 0; i < avail_entry_index; i++) {
            if (table_pages[i] == page) {
                frame = table_frames[i];
            }
        }
        if (frame == FRAME_INIT) {
            read_store(page);
            faults++;
            frame = avail_frame_index - 1;
        }
    }


    // insert page and frame into TLB, find physical address, find value at mem location then print
    buff_insert(page, frame);
    addr_physical = get_physical(frame, offset);
    this_val = physical_mem[frame][offset];
    printf(" %d \t\t\t\t %d \t\t\t\t %d \n", addr_virtual, addr_physical, this_val);
}

/*
 * AND the virtual address and mask then bit shift to get page.
 */
int get_page(int addr, int mask) {
    return ((addr & mask) >> BYTE);
}

/*
 * Bit shift frame then OR with offset to get physical address
 */
int get_physical(int frame, int offset) {
    return (frame << BYTE) | offset;
}

/*
 * Insert page and frame into TLB with least recently used implementation
 */
void buff_insert(int page, int frame) {
    int counter = 0;
    for (int i = 0; i < buff_entries; i++, counter++) {
        if (buff_pages[i] == page) { // move to next if already in the TLB
            break;
        }
    }
    if (counter != buff_entries) {
        for (int i = counter; i < buff_entries - 1; i++) { // shift TLB array values right one
            buff_right_shift(i);
        }
        if (buff_entries < SZ_BUFF) { // enough room to add to TLB at the end
            buff_assign_end(frame, page);
            buff_entries++;
        } else { // not enough room to add to TLB at the end
            buff_assign_prev(frame, page);
        }

    } else { // at current number of TLB entries
        if (buff_entries < SZ_BUFF) { // enough room to add to TLB at the end
            buff_assign_end(frame, page);
            buff_entries++;
        } else {
            for (int i = 0; i < SZ_BUFF - 1; i++) { // shift TLB array values right one
                buff_right_shift(i);
            }
            buff_assign_prev(frame, page); // assign values at next to last index
        }
    }
}

/*
 * Assign values for buff_pages & buff_frames at the end of TLB
 */
void buff_assign_end(int frame, int page) {
    buff_frames[buff_entries] = frame;
    buff_pages[buff_entries] = page;

}

/*
 * Assign values for buff_pages & buff_frames at buff_entries -1
 */
void buff_assign_prev(int frame, int page) {
    buff_frames[buff_entries - 1] = frame;
    buff_pages[buff_entries - 1] = page;
}

/*
 * Shift TLB values right once
 */
void buff_right_shift(int index) {
    buff_frames[index] = buff_frames[index + 1];
    buff_pages[index] = buff_pages[index + 1];
}

/*
 * Read backing store
 */
void read_store(int page) {
    seek_read_check(page);
    for (int i = 0; i < SZ_STORE; i++) {
        physical_mem[avail_frame_index][i] = bs_data[i];
    }

    table_frames[avail_entry_index] = avail_frame_index;
    table_pages[avail_entry_index] = page;

    // successful read ==> increase index for next available frame & entry
    avail_entry_index++;
    avail_frame_index++;
}

/*
 * Print error message if applicable when checking or reading store. If no error, then the data is read in
 * successfully.
 */
void seek_read_check(int page) {
    if (fseek(store_file, page * SZ_STORE, SEEK_SET) != 0) {
        printf("Error while checking backing store\n");
        exit(EXIT_FAILURE);
    }

    if (fread(bs_data, sizeof(signed char), SZ_STORE, store_file) == 0) {
        printf("Error while reading backing store\n");
        exit(EXIT_FAILURE);
    }
}
