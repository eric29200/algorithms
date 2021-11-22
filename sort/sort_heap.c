#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort_heap.h"
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
 * Get maximum item from i, j, k.
 */
static size_t max(void *data, int size, size_t item_size, int i, int j, int k,
                  int (*compare)(const void *, const void *))
{
  int m = i;

  if (j < size && compare(data + j * item_size, data + m * item_size) > 0)
    m = j;

  if (k < size && compare(data + k * item_size, data + m * item_size) > 0)
    m = k;

  return m;
}

/*
 * Heapify an array.
 */
static void heapify(void *data, int size, size_t item_size, int i, void *tmp,
                    int (*compare)(const void *, const void *))
{
  int j;

  for (;;) {
    j = max(data, size, item_size, i, 2 * i + 1, 2 * i + 2, compare);
    if (i == j)
      break;

    swap(data + i * item_size, data + j * item_size, tmp, item_size);
    i = j;
  }
}

/*
 * Heap sort.
 */
void sort_heap(void *data, int size, size_t item_size, int (*compare)(const void *, const void *))
{
  void *tmp;
  int i;

  /* check input array */
  if (!data || size == 0 || item_size < 2)
    return;

  /* allocate tmp */
  tmp = xmalloc(item_size);

  /* heapify array */
  for (i = (size - 2) / 2; i >= 0; i--)
    heapify(data, size, item_size, i, tmp, compare);

  /* retrieve minimum items */
  for (i = 0; i < size; i++) {
    swap(data, data + (size - i - 1) * item_size, tmp, item_size);
    heapify(data, size - i - 1, item_size, 0, tmp, compare);
  }

  /* free tmp */
  free(tmp);
}
