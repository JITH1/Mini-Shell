CC = gcc
CFLAGS = -Wall -Wextra

shell : main.o command.o functions.o jobs.o
	$(CC) $(CFLAGS) main.o command.o functions.o jobs.o -o shell
main.o : main.c shell.h 
	$(CC) $(CFLAGS) -c main.c
command.o : command.c shell.h
	$(CC) $(CFLAGS) -c command.c
functions.o : functions.c shell.h
	$(CC) $(CFLAGS) -c functions.c 
jobs.o : jobs.c shell.h
	$(CC) $(CFLAGS) -c jobs.c	

clean :
	rm -f *.o shell