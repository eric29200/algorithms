#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort_heap.h"
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
 * Get maximum element from i, j, k.
 */
static size_t max(void *data, size_t size, size_t element_size, size_t i, size_t j, size_t k,
                  int (*compare)(const void *, const void *))
{
  size_t m = i;

  if (j < size && compare(data + j * element_size, data + m * element_size) > 0)
    m = j;

  if (k < size && compare(data + k * element_size, data + m * element_size) > 0)
    m = k;

  return m;
}

/*
 * Heapify an array.
 */
static void heapify(void *data, size_t size, size_t element_size, size_t i, void *tmp,
                    int (*compare)(const void *, const void *))
{
  size_t j;

  for (;;) {
    j = max(data, size, element_size, i, 2 * i + 1, 2 * i + 2, compare);
    if (i == j)
      break;

    swap(data + i * element_size, data + j * element_size, tmp, element_size);
    i = j;
  }
}

/*
 * Heap sort.
 */
void sort_heap(void *data, size_t size, size_t element_size, int (*compare)(const void *, const void *))
{
  void *tmp;
  int i;

  /* check input array */
  if (!data || size == 0 || element_size < 2)
    return;

  /* allocate tmp */
  tmp = xmalloc(element_size);

  /* heapify array */
  for (i = (size - 2) / 2; i >= 0; i--)
    heapify(data, size, element_size, i, tmp, compare);

  /* retrieve minimum elements */
  for (i = 0; i < size; i++) {
    swap(data, data + (size - i - 1) * element_size, tmp, element_size);
    heapify(data, size - i - 1, element_size, 0, tmp, compare);
  }

  /* free tmp */
  free(tmp);
}
