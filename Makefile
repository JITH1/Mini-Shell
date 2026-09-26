CC = gcc
CFLAGS = -Wall -Wextra

minishell : main.o command.o 
	$(CC) $(CFLAGS) main.o command.o -o minishell
main.o : main.c shell.h 
	$(CC) $(CFLAGS) -c main.c
command.o : command.c shell.h
	$(CC) $(CFLAGS) -c command.c

clean :
	rm -f *.o minishell