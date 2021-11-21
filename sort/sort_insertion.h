#ifndef _SORT_INSERTION_H_
#define _SORT_INSERTION_H_

#include <stdio.h>

void sort_insertion(void *data, int size, size_t element_size, int (*compare)(const void *, const void *));

#endif
