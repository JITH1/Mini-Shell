CC = gcc
CFLAGS = -Wall -Wextra

minishell : main.o command.o functions.o
	$(CC) $(CFLAGS) main.o command.o functions.o -o minishell
main.o : main.c shell.h 
	$(CC) $(CFLAGS) -c main.c
command.o : command.c shell.h
	$(CC) $(CFLAGS) -c command.c
functions.o : functions.c shell.h
	$(CC) $(CFLAGS) -c functions.c 

clean :
	rm -f *.o minishell