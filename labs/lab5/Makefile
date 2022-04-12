comp = gcc -pthread -o
flags = -Wall -Werror
nums = 90 81 78 95 79 72 85 80

all: multithread

multithread:
	$(comp) multithread multi_thread.c

run:
	./multithread $(nums)


clean:
	rm multithread