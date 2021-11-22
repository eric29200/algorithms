#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#include <stdio.h>

struct hash_table_item_t {
  void *key;
  void *data;
  struct hash_table_item_t *next;
};

struct hash_table_t {
  size_t size;
  size_t capacity;
  struct hash_table_item_t **items;
  unsigned int (*hash_func)(const void *);
  int (*equal_func)(const void *, const void *);
};

struct hash_table_t *hash_table_create(size_t capacity, unsigned int (*hash_func)(const void *),
                                       int (*equal_func)(const void *, const void *));
void hash_table_free(struct hash_table_t *hash_table);
void *hash_table_put(struct hash_table_t *hash_table, void *key, void *data);
void *hash_table_get(struct hash_table_t *hash_table, void *key);
void *hash_table_remove(struct hash_table_t *hash_table, void *key);

#endif
