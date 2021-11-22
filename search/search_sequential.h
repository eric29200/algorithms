#ifndef _SEARCH_SEQUENTIAL_H_
#define _SEARCH_SEQUENTIAL_H_

#include <stdio.h>

int search_sequential(void *data, int size, size_t item_size, void *key, int (*equal_func)(const void *, const void *));

#endif
