CFLAGS  := -Wall -O2 -g
CC      := gcc

all: algo

algo: utils/mem.o compression/huffman.o compression/lz77.o compression/lz78.o data_structures/array_list.o data_structures/list.o data_structures/queue.o data_structures/trie.o \
      data_structures/heap.o plot/plot.o stats/kmeans.o algo.o
	$(CC) $(CFLAGS) -o $@ $^

.o: .c
	$(CC) $(CFLAGS) -c $^

clean :
	rm -f *.o */*.o algo
