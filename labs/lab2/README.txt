Maggie Horton
CS-451 Summer 2020
Lab 2
-------------------------------------------------------------------------------------------------
ABOUT
This lab consists of two tasks.
	TASK1: myshell.c calls mysort.c to sort numbers, which will either print the
		   sorted values to stdout or will write them to a file.
	TASK2: server.c & client.c are run in different terminals in order to establish
		   a 'chat' using IPC via. FIFOs.
-------------------------------------------------------------------------------------------------
COMPILATION
In the project directory, type 'make' into the terminal in order to compile mysort.c, myshell.c,
client.c & server.c.
-------------------------------------------------------------------------------------------------
RUNNING
	TASK1: Type './myshell' into the terminal in order to run the code for task 1.
	TASK2: Open two terminals. In the first, type './server' and in the second type
		   './client'. It is important for server.c to be run before client.c.
-------------------------------------------------------------------------------------------------
USAGE
	TASK1: When "myprompt> " appears you have two options. (1) type 'mysort input_file_name' in
		   order to have the output sent to stdout. (2) type 'mysort input_file_name >
		   output_file_name' in order to have the output written to a file.
	TASK2: When "You: " appears in server.c, type a message and hit enter. After, you may do the
		   same in client.c. This will keep going until the programs exit. In order to exit the
		   program, type 'quit' into the client's prompt.