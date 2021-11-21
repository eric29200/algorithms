#ifndef _SORT_MERGE_H_
#define _SORT_MERGE_H_

#include <stdio.h>

void sort_merge(void *data, int size, size_t element_size, int (*compare)(const void *, const void *));

#endif
