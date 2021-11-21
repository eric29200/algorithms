#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort_merge.h"
#include "../utils/mem.h"

/*
 * Recursive merge sort.
 */
static void __sort_merge(void *data, int i, int j, size_t element_size, void *tmp,
                         int (*compare)(const void *, const void *))
{
  int mid, k, ptr_left, ptr_right;

  if (j <= i)
    return;

  /* divide data in 2 partitions */
  mid = (i + j) / 2;
  __sort_merge(data, i, mid, element_size, tmp, compare);
  __sort_merge(data, mid + 1, j, element_size, tmp, compare);

  /* merge sorted partitions into tmp array */
  for (k = i, ptr_left = i, ptr_right = mid + 1; k <= j; k++) {
    if (ptr_left == mid + 1)
      memcpy(tmp + k * element_size, data + ptr_right++ * element_size, element_size);
    else if (ptr_right == j + 1)
      memcpy(tmp + k * element_size, data + ptr_left++ * element_size, element_size);
    else if (compare(data + ptr_left * element_size, data + ptr_right * element_size) < 0)
      memcpy(tmp + k * element_size, data + ptr_left++ * element_size, element_size);
    else
      memcpy(tmp + k * element_size, data + ptr_right++ * element_size, element_size);
  }

  /* copy tmp array */
  memcpy(data + i * element_size, tmp + i * element_size, (j - i + 1) * element_size);
}

/*
 * Merge sort.
 */
void sort_merge(void *data, int size, size_t element_size, int (*compare)(const void *, const void *))
{
  void *tmp;

  /* check input array */
  if (!data || size == 0 || element_size < 2)
    return;

  /* allocate tmp array */
  tmp = xmalloc(size * element_size);

  /* merge sort */
  __sort_merge(data, 0, size - 1, element_size, tmp, compare);

  /* free tmp array */
  free(tmp);
}
