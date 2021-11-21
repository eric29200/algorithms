#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort_insertion.h"
#include "../utils/mem.h"

/*
 * Insertion sort.
 */
void sort_insertion(void *data, size_t size, size_t element_size, int (*compare)(const void *, const void *))
{
  void *key;
  int i, j;

  /* check input array */
  if (!data || size == 0 || element_size < 2)
    return;

  /* allocate key */
  key = xmalloc(element_size);

  /* insert each element */
  for (i = 1; i < size; i++) {
    /* copy key */
    memcpy(key, data + i * element_size, element_size);

    /* find position and shift elements on the right */
    for (j = i - 1; j >= 0 && compare(data + j * element_size, key) > 0; j--)
      memcpy(data + (j + 1) * element_size, data + j * element_size, element_size);

    /* insert key */
    memcpy(data + (j + 1) * element_size, key, element_size);
  }

  /* free key */
  free(key);
}
