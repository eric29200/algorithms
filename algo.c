#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "list.h"

#define NB_ELEMENTS   10000

/*
 * Integer comparison.
 */
static inline int int_compare(const void *i1, const void *i2)
{
  return *((int *) i1) - *((int *) i2);
}

int main()
{
  struct list_t *list = NULL;
  int data[NB_ELEMENTS], i;

  /* create elements and list */
  for (i = 0; i < NB_ELEMENTS; i++) {
    data[i] = rand() % 100;
    list = list_prepend(list, &data[i]);
  }

  /* sort */
  list_sort(list, int_compare);

  return 0;
}
