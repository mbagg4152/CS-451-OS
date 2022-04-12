#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

int
main(int argc, char *argv[]) {
    struct pstat st;

    if (argc != 2) {
        printf(1, "usage: test counter");
        exit();
    }

    int i, x, l, j;
    int mypid = getpid();

    for (i = 1; i < atoi(argv[1]); i++) {
        x = x + i;
    }

    getpinfo(&st);
    for (j = 0; j < NPROC; j++) {
        if (st.inuse[j] && st.pid[j] == mypid) {
            for (l = 3; l >= 0; l--) {
                printf(1, "Total time used at P%d = %d ticks\n", l, st.ticks[j][l]);
            }
        }
    }
    exit();
    return 0;
}


/*
OUTPUT:

If you run test 10000000 the expected output is something like below:

level:3      ticks-used:8
level:2      ticks-used:16
level:1      ticks-used:32
level:0      ticks-used:160

If you invoke with small counter value such as 10 (test 10), 
then the output should be like this:

level:3      ticks-used:1
level:2      ticks-used:0
level:1      ticks-used:0
level:0      ticks-used:0


The ticks used on the last level will be somewhat unpredictable and it may vary.

*/