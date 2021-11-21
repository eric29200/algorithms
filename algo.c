#include <stdio.h>
#include <stdlib.h>

#include "data_structures/hash_table.h"
#include "sort/sort_quick.h"
#include "search/search_sequential.h"
#include "search/search_binary.h"
#include "utils/mem.h"

#define NB_TESTS          10000
#define NB_ELEMENTS       200000

/*
 * Hash an integer.
 */
static inline int hash_int(const void *data)
{
  return *((int *) data);
}

/*
 * Integer equality.
 */
static inline int equal_int(const void *a1, const void *a2)
{
  return *((int *) a1) == *((int *) a2);
}

/*
 * Integer comparison.
 */
static inline int compare_int(const void *a1, const void *a2)
{
  return *((int *) a1) - *((int *) a2);
}

/*
 * Main.
 */
int main()
{
  int *data, i, key;

  /* create data */
  data = (int *) xmalloc(sizeof(int) * NB_ELEMENTS);
  for (i = 0; i < NB_ELEMENTS; i++)
    data[i] = rand();

  /* generate random key */
  key = rand();

  /* sort data */
  sort_quick(data, NB_ELEMENTS, sizeof(int), compare_int);

  /* binary search */
  for (i = 0; i < NB_TESTS; i++)
    search_binary(data, NB_ELEMENTS, sizeof(int), &key, compare_int);

  /* free elements */
  free(data);

  return 0;
}
