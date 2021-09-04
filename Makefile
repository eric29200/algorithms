CFLAGS  := -Wall -O2
CC      := gcc

all: algo

algo: algo.o list.o queue.o heap.o trie.o huffman.o lz77.o lz78.o
	$(CC) $(CFLAGS) -o $@ $^

.o: .c
	$(CC) $(CFLAGS) -c $^

clean :
	rm -f *.o algo
