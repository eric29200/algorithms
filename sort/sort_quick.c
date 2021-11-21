#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/mem.h"

/*
 * Swap 2 elements.
 */
static inline void swap(void *e1, void *e2, void *tmp, size_t element_size)
{
  memcpy(tmp, e1, element_size);
  memcpy(e1, e2, element_size);
  memcpy(e2, tmp, element_size);
}

/*
 * Recursive quick sort.
 */
static void __quick_sort(void *data, size_t size, size_t element_size, void *tmp,
                         int (*compare)(const void *, const void *))
{
  void *pivot;
  size_t i, j;

  if (size < 2)
    return;

  /* choose pivot */
  pivot = data + (size / 2) * element_size;

  /* sort */
  for (i = 0, j = size - 1;; i++, j--) {
    /* find misplaced element in left partition */
    while (compare(data + i * element_size, pivot) < 0)
      i++;

    /* find misplaced element in right partition */
    while (compare(data + j * element_size, pivot) > 0)
      j--;

    if (i >= j)
      break;

    /* swap elements */
    swap(data + i * element_size, data + j * element_size, tmp, element_size);
  }

  /* quicksort remaining */
  __quick_sort(data, i, element_size, tmp, compare);
  __quick_sort(data + i * element_size, size - i, element_size, tmp, compare);
}

/*
 * Quick sort.
 */
void sort_quick(void *data, size_t size, size_t element_size, int (*compare)(const void *, const void *))
{
  void *tmp;

  /* check input array */
  if (!data || size == 0 || element_size < 2)
    return;

  /* allocate tmp */
  tmp = xmalloc(element_size);

  /* quick sort */
  __quick_sort(data, size, element_size, tmp, compare);

  /* free tmp */
  free(tmp);
}
