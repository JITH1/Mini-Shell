CC = gcc
CFLAGS = -Wall -Wextra

shell : main.o command.o functions.o jobs.o pipe.o
	$(CC) $(CFLAGS) main.o command.o functions.o jobs.o pipe.o -o shell
main.o : main.c shell.h 
	$(CC) $(CFLAGS) -c main.c
command.o : command.c shell.h
	$(CC) $(CFLAGS) -c command.c
functions.o : functions.c shell.h
	$(CC) $(CFLAGS) -c functions.c 
jobs.o : jobs.c shell.h
	$(CC) $(CFLAGS) -c jobs.c	
pipe.o : pipe.c shell.h
	$(CC) $(CFLAGS) -c pipe.c

clean :
	rm -f *.o shell
