CFLAGS  := -Wall -O3
CC      := gcc

all: sort

sort: sort.o list.o
	$(CC) $(CFLAGS) -o $@ $^

.o: .c
	$(CC) $(CFLAGS) -c $^

clean :
	rm -f *.o sort
