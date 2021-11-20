#ifndef _SORT_BUBBLE_H_
#define _SORT_BUBBLE_H_

#include <stdio.h>

void sort_bubble(void *data, size_t size, size_t element_size, int (*compare)(const void *, const void *));

#endif
