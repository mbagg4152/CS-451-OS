all: myshell mysort client server

myshell: myshell.c
	gcc -o myshell myshell.c

mysort: mysort.c
	gcc -o mysort mysort.c

client: client.c
	gcc -o client client.c

server: server.c
	gcc -o server server.c

clean:
	rm myshell
	rm mysort
	rm server
	rm client