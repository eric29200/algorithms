#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sort/sort_bubble.h"
#include "sort/sort_insertion.h"
#include "sort/sort_quick.h"
#include "sort/sort_merge.h"
#include "sort/sort_heap.h"
#include "utils/mem.h"

#define NB_ELEMENTS   20000000

/*
 * Integer comparison.
 */
static inline int int_compare(const void *a1, const void *a2)
{
  return *((int *) a1) - *((int *) a2);
}

/*
 * Sort test.
 */
static void sort_test(void *data, const char *method, void (*sort)(void *, size_t, size_t, int (*compare)()))
{
  clock_t start, end;
  double time;
  void *tmp;

  /* copy data */
  tmp = xmalloc(NB_ELEMENTS * sizeof(int));
  memcpy(tmp, data, NB_ELEMENTS * sizeof(int));

  /* sort */
  start = clock();
  sort(tmp, NB_ELEMENTS, sizeof(int), int_compare);
  end = clock();
  time = (double) (end - start) / CLOCKS_PER_SEC;

  /* free tmp data */
  free(tmp);

  /* print statistics */
  printf("******* %s\n", method);
  printf("Sort time : %f\n", time);
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

  /* heap sort */
  sort_test(data, "Heap sort", sort_heap);

  /* quick sort */
  sort_test(data, "Quick sort", sort_quick);

  /* merge sort */
  sort_test(data, "Merge sort", sort_merge);

  /* free data */
  free(data);

  return 0;
}
