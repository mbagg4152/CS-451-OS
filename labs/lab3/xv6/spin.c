#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
    int sum = 0;
    for (int i = 0; i < atoi(argv[1]); i++) {
        sum = sum + i;
    }
    exit();
}