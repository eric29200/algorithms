CFLAGS  := -Wall -Wextra -O2 -g
CC      := gcc

all: algo

algo: compression/huffman.o compression/lz77.o compression/lz78.o \
      data_structures/array_list.o data_structures/list.o data_structures/queue.o data_structures/trie.o data_structures/heap.o \
      data_structures/tree.o data_structures/hash_table.o data_structures/graph.o data_structures/priority_queue.o \
      sort/sort_bubble.o sort/sort_insertion.o sort/sort_heap.o sort/sort_quick.o sort/sort_merge.o \
      search/search_sequential.o search/search_binary.o \
      geometry/geometry.o geometry/point.o geometry/line_string.o geometry/polygon.o geometry/envelope.o geometry/wkb_reader.o \
      utils/mem.o utils/math.o \
      plot/plot.o \
      stats/kmeans.o \
      algo.o
	$(CC) $(CFLAGS) -o $@ $^

.o: .c
	$(CC) $(CFLAGS) -c $^

clean :
	rm -f *.o */*.o algo
