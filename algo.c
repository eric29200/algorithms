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

/*
 * Sort a list.
 */
static void sort(struct list_t *list,
                 void (*sort_func)(struct list_t *, int (*compare_func)(const void *, const void *)), char *name)
{
  struct list_t *new_list;
  clock_t start, end;

  /* copy list */
  new_list = list_copy(list);
  if (!new_list)
    return;

  /* sort */
  start = clock();
  sort_func(new_list, int_compare);
  end = clock();

  /* free list */
  list_free(new_list);

  /* print stats */
  printf("%s : %f\n", name, (end - start) / (double) CLOCKS_PER_SEC);
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

  /* bubble sort */
  sort(list, list_sort_bubble, "Bubble sort");

  /* insertion sort */
  sort(list, list_sort_bubble, "Insertion sort");

  return 0;
}
