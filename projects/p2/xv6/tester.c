#include "types.h"
#include "stat.h"
#include "user.h"

void waste_time(int val);

int main(int argc, char *argv[]) {
	int pid = fork();
	int val = 0;
	if (argc == 2) {
		val = (int)argv[1];
	} else {
		val = 2;
	}
	if (pid < 0) {
		return 1;
	} else if (pid == 0) {
		waste_time(val);
		return 0;
	} else {
		wait();
	}
	return 0;
}

void waste_time(int val) {
	float fv = (float) val;
	for (float f = 0; f < 8000000.0; f += 0.01) {
		fv = fv + 3.14 * 89.64;
	}
	return;
}