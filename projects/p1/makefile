c=gcc -o
f=#-Wall -Werror


all: myshell my-cat my-uniq my-wc

myshell: myshell.c
	gcc -o myshell myshell.c $(f)

my-cat: my-cat.c
	gcc -o my-cat my-cat.c $(f)

my-uniq: my-uniq.c
	gcc -o my-uniq my-uniq.c $(f)

my-wc: my-wc.c
	gcc -o my-wc my-wc.c $(f)

clean:
	rm myshell
	rm my-cat
	rm my-uniq
	rm my-wc