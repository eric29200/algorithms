#ifndef _SORT_QUICK_H_
#define _SORT_QUICK_H_

#include <stdio.h>

void sort_quick(void *data, size_t size, size_t element_size, int (*compare)(const void *, const void *));

#endif
