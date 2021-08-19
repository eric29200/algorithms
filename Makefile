CFLAGS  := -Wall -O3
CC      := gcc

all: algo

algo: algo.o list.o queue.o
	$(CC) $(CFLAGS) -o $@ $^

.o: .c
	$(CC) $(CFLAGS) -c $^

clean :
	rm -f *.o algo
