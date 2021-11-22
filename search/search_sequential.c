#include <stdio.h>

#include "search_sequential.h"

/*
 * Sequantial search.
 */
int search_sequential(void *data, int size, size_t item_size, void *key, int (*equal_func)(const void *, const void *))
{
  int i;

  for (i = 0; i < size; i++)
    if (equal_func(key, data + i * item_size))
      return 1;

  return 0;
}
