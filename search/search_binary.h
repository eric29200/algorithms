#ifndef _BINARY_SEARCH_H_
#define _BINARY_SEARCH_H_

#include <stdio.h>

int search_binary(void *data, int size, size_t element_size, void *key, int (*compare_func)(const void *, const void *));

#endif
