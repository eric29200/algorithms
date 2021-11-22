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
  char change = 1;
  void *tmp;
  int i;

  /* check input array */
  if (!data || size == 0 || item_size < 2)
    return;

  /* allocate tmp */
  tmp = xmalloc(item_size);

  /* compute until no changes occur */
  while (change) {
    change = 0;

    /* swap items 2 by 2 */
    for (i = 1; i < size; i++) {
      if (compare(data + (i - 1) * item_size, data + i * item_size) > 0) {
        swap(data + (i - 1) * item_size, data + i * item_size, tmp, item_size);
        change = 1;
      }
    }
  }

  /* free tmp */
  free(tmp);
}
