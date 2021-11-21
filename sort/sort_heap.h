#ifndef _SORT_HEAP_H_
#define _SORT_HEAP_H_

#include <stdio.h>

void sort_heap(void *data, int size, size_t element_size, int (*compare)(const void *, const void *));

#endif
