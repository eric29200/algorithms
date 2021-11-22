#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/mem.h"

/*
 * Swap 2 items.
 */
static inline void swap(void *e1, void *e2, void *tmp, size_t item_size)
{
  memcpy(tmp, e1, item_size);
  memcpy(e1, e2, item_size);
  memcpy(e2, tmp, item_size);
}

/*
 * Recursive quick sort.
 */
static void __quick_sort(void *data, int size, size_t item_size, void *tmp,
                         int (*compare)(const void *, const void *))
{
  void *pivot;
  int i, j;

  if (size < 2)
    return;

  /* choose pivot */
  pivot = data + (size / 2) * item_size;

  /* sort */
  for (i = 0, j = size - 1;; i++, j--) {
    /* find misplaced item in left partition */
    while (compare(data + i * item_size, pivot) < 0)
      i++;

    /* find misplaced item in right partition */
    while (compare(data + j * item_size, pivot) > 0)
      j--;

    if (i >= j)
      break;

    /* swap items */
    swap(data + i * item_size, data + j * item_size, tmp, item_size);
  }

  /* quicksort remaining */
  __quick_sort(data, i, item_size, tmp, compare);
  __quick_sort(data + i * item_size, size - i, item_size, tmp, compare);
}

/*
 * Quick sort.
 */
void sort_quick(void *data, int size, size_t item_size, int (*compare)(const void *, const void *))
{
  void *tmp;

  /* check input array */
  if (!data || size == 0 || item_size < 2)
    return;

  /* allocate tmp */
  tmp = xmalloc(item_size);

  /* quick sort */
  __quick_sort(data, size, item_size, tmp, compare);

  /* free tmp */
  free(tmp);
}
