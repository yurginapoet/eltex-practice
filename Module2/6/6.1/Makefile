CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

TARGET = contacts
LIB = libcontacts.a

all: $(TARGET)

$(TARGET): main.o $(LIB)
	$(CC) $(CFLAGS) -o $@ main.o -L. -lcontacts

$(LIB): contacts.o
	ar rcs $@ $^

main.o: main.c contacts.h
	$(CC) $(CFLAGS) -c main.c

contacts.o: contacts.c contacts.h
	$(CC) $(CFLAGS) -c contacts.c

clean:
	rm -f *.o $(TARGET) $(LIB)
