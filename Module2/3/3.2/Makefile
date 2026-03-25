CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

all: prog

prog: main.o ip_utils.o
	$(CC) $(CFLAGS) -o prog main.o ip_utils.o

main.o: main.c ip_utils.h
	$(CC) $(CFLAGS) -c main.c

ip_utils.o: ip_utils.c ip_utils.h
	$(CC) $(CFLAGS) -c ip_utils.c

clean:
	rm -f *.o prog