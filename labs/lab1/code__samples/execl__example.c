
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv) {

  execl("/bin/ls", "ls", "-l", NULL);

  return 0;
}
