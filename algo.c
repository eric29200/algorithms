#include <stdio.h>
#include <stdlib.h>

#include "data_structures/hash_table.h"

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
 * Main.
 */
int main()
{
  struct hash_table_t *hash_table;
  int data[10];

  hash_table = hash_table_create(10, hash_int, equal_int);

  data[0] = 1;
  data[1] = 2;
  data[2] = 11;
  printf("%d\n", hash_table_put(hash_table, &data[0], &data[0]) == NULL);
  printf("%d\n", hash_table_put(hash_table, &data[1], &data[1]) == NULL);
  printf("%d\n", hash_table_put(hash_table, &data[2], &data[2]) == NULL);
  printf("%d\n", hash_table_put(hash_table, &data[2], &data[2]) == NULL);

  hash_table_remove(hash_table, &data[2]);
  hash_table_remove(hash_table, &data[2]);
  hash_table_remove(hash_table, &data[2]);

  hash_table_free(hash_table);

  return 0;
}
