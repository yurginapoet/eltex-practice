CC=gcc
CFLAGS=-Wall -Wextra -std=c11

TARGET=calc

all: $(TARGET)

$(TARGET): main.o calc.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o calc.o

main.o: main.c calc.h
	$(CC) $(CFLAGS) -c main.c

calc.o: calc.c calc.h
	$(CC) $(CFLAGS) -c calc.c

clean:
	rm -f *.o $(TARGET)