#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort_insertion.h"
#include "../utils/mem.h"

/*
 * Insertion sort.
 */
void sort_insertion(void *data, int size, size_t item_size, int (*compare)(const void *, const void *))
{
  void *key;
  int i, j;

  /* check input array */
  if (!data || size == 0 || item_size < 2)
    return;

  /* allocate key */
  key = xmalloc(item_size);

  /* insert each item */
  for (i = 1; i < size; i++) {
    /* copy key */
    memcpy(key, data + i * item_size, item_size);

    /* find position and shift items on the right */
    for (j = i - 1; j >= 0 && compare(data + j * item_size, key) > 0; j--)
      memcpy(data + (j + 1) * item_size, data + j * item_size, item_size);

    /* insert key */
    memcpy(data + (j + 1) * item_size, key, item_size);
  }

  /* free key */
  free(key);
}
