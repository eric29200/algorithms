#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort_bubble.h"
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
 * Bubble sort.
 */
void sort_bubble(void *data, int size, size_t item_size, int (*compare)(const void *, const void *))
{
  void *tmp;
  int i, j;

  /* check input array */
  if (!data || size == 0 || item_size < 2)
    return;

  /* allocate tmp */
  tmp = xmalloc(item_size);

  /* bubble sort */
  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++)
      if (compare(data + i * item_size, data + j * item_size) < 0)
        swap(data + i * item_size, data + j * item_size, tmp, item_size);

  /* free tmp */
  free(tmp);
}
