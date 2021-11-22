#ifndef _SORT_QUICK_H_
#define _SORT_QUICK_H_

#include <stdio.h>

void sort_quick(void *data, int size, size_t item_size, int (*compare)(const void *, const void *));

#endif
