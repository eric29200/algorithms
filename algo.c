#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "heap.h"

#define NB_ELEMENTS   100

/*
 * Integer comparison.
 */
static inline int int_compare(const void *i1, const void *i2)
{
  return *((int *) i1) - *((int *) i2);
}

int main()
{
  int data[NB_ELEMENTS];
  struct heap_t *heap;
  void *max;
  size_t i;

  /* create a heap */
  heap = heap_create(HEAP_MAX, NB_ELEMENTS, int_compare);

  /* create elements */
  for (i = 0; i < NB_ELEMENTS; i++) {
    data[i] = rand() % 100;
    heap_insert(heap, &data[i]);
  }

  while (1) {
    max = heap_max(heap);
    if (!max)
      break;

    printf("%d\n", *((int *) max));
  }

  /* free heap */
  heap_free(heap);

  return 0;
}
