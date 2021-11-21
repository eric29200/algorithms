#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort_bubble.h"
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
 * Bubble sort.
 */
void sort_bubble(void *data, size_t size, size_t element_size, int (*compare)(const void *, const void *))
{
  char change = 1;
  void *tmp;
  size_t i;

  /* check input array */
  if (!data || size == 0 || element_size < 2)
    return;

  /* allocate tmp */
  tmp = xmalloc(element_size);

  /* compute until no changes occur */
  while (change) {
    change = 0;

    /* swap elements 2 by 2 */
    for (i = 1; i < size; i++) {
      if (compare(data + (i - 1) * element_size, data + i * element_size) > 0) {
        swap(data + (i - 1) * element_size, data + i * element_size, tmp, element_size);
        change = 1;
      }
    }
  }

  /* free tmp */
  free(tmp);
}
