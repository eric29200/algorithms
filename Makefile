CFLAGS  := -Wall -O3
CC      := gcc

all: sort

sort: sort.o slist.o stack.o queue.o
	$(CC) $(CFLAGS) -o $@ $^

.o: .c
	$(CC) $(CFLAGS) -c $^

clean :
	rm -f *.o sort
