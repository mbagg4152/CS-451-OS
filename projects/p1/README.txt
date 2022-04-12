Maggie Horton
CS-451 Summer 2020
Project 1
-------------------------------------------------------------------------------------------------
ABOUT
This project consists of 4 c files: myshell.c, my-cat.c, my-uniq.c & my-wc.c. They are described
below:
	myshell: A simple shell program that can call 3 commands: my-cat, my-uniq & my-wc. Can be
	used to do simple commands or can be used to pipe commands together (max of 2 times)

	my-cat: Takes in a text file or data piped from stdin & performs similarly to the linux
	command cat. It takes each char from the file and then displays it to stdout.

	my-uniq: Takes in a text file or data piped from stdin & performs similarly to the linux
	command uniq. It looks at each line in the file and removes any duplicates.

	my-wc: Takes in a text file or data piped from stdin & performs similarly to the linux
	command wc. It counts the number of lines & words present in a file.
-------------------------------------------------------------------------------------------------
COMPILATION
In the project directory, type 'make' into the terminal in order to compile myshell, my-cat,
my-uniq & my-wc.
-------------------------------------------------------------------------------------------------
RUNNING
Type './myshell' into the terminal in order to start the program. my-cat, my-uniq & my-wc
can be run in the same way but they would need a file path supplied as a commandline arg.
-------------------------------------------------------------------------------------------------
USAGE
	NO PIPES: when the 'myprompt> ' prompt shows up type in one of the following:
			  my-cat <txt_path>
			  my-uniq <txt_path>
			  my-wc <txt_path>
	SINGLE PIPES: in the prompt type 'cmd1 <txt_path> | cmd2' in order for cmd1's output to be
	directed to cmd2.
	DOUBLE PIPES: in the prompt type 'cmd1 <txt_path> | cmd2 | cmd3' in order for cmd1's output
	to be directed to cmd2 and then for cmd2's output to be directed to cmd3.