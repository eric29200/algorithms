#include <stdio.h>

#include "search_binary.h"

/*
 * Recursive binary search.
 */
static int __search_binary(void *data, int l, int r, size_t element_size, void *key,
                           int (*compare_func)(const void *, const void *))
{
  int mid, res;

  if (l > r)
    return 0;

  /* get middle element */
  mid = l + (r - l) / 2;

  /* compare key with middle element */
  res = compare_func(key, data + mid * element_size);

  /* key = middle element */
  if (res == 0)
    return 1;

  /* key < middle element */
  if (res < 0)
    return __search_binary(data, l, mid - 1, element_size, key, compare_func);

  /* key > middle element */
  return __search_binary(data, mid + 1, r, element_size, key, compare_func);
}

/*
 * Binary search (data must be sorted).
 */
int search_binary(void *data, int size, size_t element_size, void *key, int (*compare_func)(const void *, const void *))
{
  return __search_binary(data, 0, size - 1, element_size, key, compare_func);
}
