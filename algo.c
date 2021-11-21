#include <stdio.h>
#include <stdlib.h>

#include "sort/sort_bubble.h"
#include "sort/sort_insertion.h"
#include "sort/sort_quick.h"
#include "sort/sort_merge.h"
#include "utils/mem.h"

#define NB_ELEMENTS   20

/*
 * Integer comparison.
 */
static inline int int_compare(const void *a1, const void *a2)
{
  return *((int *) a1) - *((int *) a2);
}

/*
 * Main.
 */
int main(int argc, char **argv)
{
  int *data;
  size_t i;

  /* create elements */
  data = (int *) xmalloc(sizeof(int) * NB_ELEMENTS);
  for (i = 0; i < NB_ELEMENTS; i++)
    data[i] = rand() % 10;

  /* print data */
  for (i = 0; i < NB_ELEMENTS; i++)
    printf("%d|", data[i]);
  printf("\n");

  /* sort array */
  sort_merge(data, NB_ELEMENTS, sizeof(int), int_compare);

  /* print data */
  for (i = 0; i < NB_ELEMENTS; i++)
    printf("%d|", data[i]);
  printf("\n");

  /* free data */
  free(data);

  return 0;
}
