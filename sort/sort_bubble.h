#ifndef _SORT_BUBBLE_H_
#define _SORT_BUBBLE_H_

#include <stdio.h>

void sort_bubble(void *data, int size, size_t item_size, int (*compare)(const void *, const void *));

#endif
