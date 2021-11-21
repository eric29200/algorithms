#include <stdio.h>
#include <stdlib.h>

#include "hash_table.h"
#include "../utils/mem.h"

/*
 * Create a hash table.
 */
struct hash_table_t *hash_table_create(size_t capacity, unsigned int (*hash_func)(const void *),
                                       int (*equal_func)(const void *, const void *))
{
  struct hash_table_t *hash_table;
  size_t i;

  hash_table = (struct hash_table_t *) xmalloc(sizeof(struct hash_table_t));
  hash_table->size = 0;
  hash_table->capacity = capacity;
  hash_table->hash_func = hash_func;
  hash_table->equal_func = equal_func;
  hash_table->elements = (struct hash_table_element_t **) xmalloc(sizeof(struct hash_table_element_t *) * capacity);
  for (i = 0; i < capacity; i++)
    hash_table->elements[i] = NULL;

  return hash_table;
}

/*
 * Free a hash table.
 */
void hash_table_free(struct hash_table_t *hash_table)
{
  struct hash_table_element_t *elt, *next;
  size_t i;

  if (!hash_table)
    return;

  if (hash_table->elements) {
    for (i = 0; i < hash_table->capacity; i++) {
      for (elt = hash_table->elements[i]; elt != NULL;) {
        next = elt->next;
        free(elt);
        elt = next;
      }
    }

    free(hash_table->elements);
  }

  free(hash_table);
}

/*
 * Put data in a hash table.
 */
void *hash_table_put(struct hash_table_t *hash_table, void *key, void *data)
{
  struct hash_table_element_t *elt;
  unsigned int hash;
  void *ret;

  if (!hash_table)
    return NULL;

  /* compute hash */
  hash = hash_table->hash_func(key) % hash_table->capacity;

  /* check if key is already present */
  for (elt = hash_table->elements[hash]; elt != NULL; elt = elt->next) {
    if (hash_table->equal_func(elt->key, key)) {
      ret = elt->data;
      elt->data = data;
      return ret;
    }
  }

  /* create new element */
  elt = (struct hash_table_element_t *) xmalloc(sizeof(struct hash_table_element_t));
  elt->key = key;
  elt->data = data;
  elt->next = hash_table->elements[hash];
  hash_table->elements[hash] = elt;
  hash_table->size++;

  return NULL;
}

/*
 * Get data from a hash table.
 */
void *hash_table_get(struct hash_table_t *hash_table, void *key)
{
  struct hash_table_element_t *elt;
  unsigned int hash;

  if (!hash_table)
    return NULL;

  /* compute hash */
  hash = hash_table->hash_func(key) % hash_table->capacity;

  /* seach for key */
  for (elt = hash_table->elements[hash]; elt != NULL; elt = elt->next)
    if (hash_table->equal_func(elt->key, key))
      return elt->data;

  return NULL;
}

/*
 * Remove data from a hash table.
 */
void *hash_table_remove(struct hash_table_t *hash_table, void *key)
{
  struct hash_table_element_t *elt, *prev;
  unsigned int hash;
  void *ret;

  if (!hash_table)
    return NULL;

  /* compute hash */
  hash = hash_table->hash_func(key) % hash_table->capacity;

  /* seach for key */
  for (elt = hash_table->elements[hash], prev = NULL; elt != NULL; elt = elt->next) {
    if (hash_table->equal_func(elt->key, key)) {
      ret = elt->data;

      if (prev)
        prev->next = elt->next;
      else
        hash_table->elements[hash] = elt->next;

      free(elt);
      hash_table->size--;

      return ret;
    }

    prev = elt;
  }

  return NULL;
}
