CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

all: prog

prog: main.o permissions.o fperm.o
	$(CC) $(CFLAGS) -o prog main.o permissions.o fperm.o

main.o: main.c permissions.h fperm.h
	$(CC) $(CFLAGS) -c main.c

permissions.o: permissions.c permissions.h
	$(CC) $(CFLAGS) -c permissions.c

fperm.o: fperm.c fperm.h
	$(CC) $(CFLAGS) -c fperm.c

clean:
	rm -f *.o prog