#include <stdlib.h>

#include "array_list.h"
#include "../utils/mem.h"

#define ARRAY_LIST_GROW_SIZE      100

/*
 * Grow an array list if needed.
 */
static void __array_list_grow(struct array_list_t *list)
{
  if (!list || list->size < list->capacity)
    return;

  list->capacity += ARRAY_LIST_GROW_SIZE;
  list->items = (void **) xrealloc(list->items, sizeof(void *) * list->capacity);
}

/*
 * Create an array list.
 */
struct array_list_t *array_list_create()
{
  struct array_list_t *list;

  list = (struct array_list_t *) xmalloc(sizeof(struct array_list_t));
  list->items = NULL;
  list->size = 0;
  list->capacity = 0;

  return list;
}

/*
 * Free an array list.
 */
void array_list_free(struct array_list_t *list)
{
  if (!list)
    return;

  xfree(list->items);
  free(list);
}

/*
 * Free an array list and its items.
 */
void array_list_free_full(struct array_list_t *list, void (*free_func)(void *))
{
  size_t i;

  if (!list)
    return;

  if (list->items) {
    for (i = 0; i < list->size; i++)
      free_func(list->items[i]);

    free(list->items);
  }
  free(list);
}

/*
 * Clear an array list.
 */
void array_list_clear(struct array_list_t *list)
{
  if (!list)
    return;

  xfree(list->items);
  list->items = NULL;
  list->size = 0;
  list->capacity = 0;
}

/*
 * Add an item at the end of a list.
 */
void array_list_add(struct array_list_t *list, void *item)
{
  if (!list)
    return;

  /* grow array list if needed */
  __array_list_grow(list);

  /* add item */
  list->items[list->size++] = item;
}

/*
 * Add an item at index (shift other items on the right).
 */
void array_list_add_idx(struct array_list_t *list, size_t idx, void *item)
{
  size_t i;

  if (!list || idx > list->size)
    return;

  /* grow array list if needed */
  __array_list_grow(list);

  /* shif items on the right */
  for (i = list->size; i > idx; i--)
    list->items[i] = list->items[i - 1];

  /* add item */
  list->items[idx] = item;
  list->size++;
}

/*
 * Set an item in a list.
 */
void *array_list_set(struct array_list_t *list, size_t idx, void *item)
{
  void *ret;

  if (!list || idx >= list->size)
    return NULL;

  ret = list->items[idx];
  list->items[idx] = item;
  return ret;
}

/*
 * Remove an item in a list.
 */
void *array_list_remove(struct array_list_t *list, size_t idx)
{
  void *ret;
  size_t i;

  if (!list || idx >= list->size)
    return NULL;

  /* save item */
  ret = list->items[idx];

  /* shift items on the left */
  for (i = idx; i < list->size - 1; i++)
    list->items[i] = list->items[i + 1];

  list->size--;
  return ret;
}

/*
 * Sort an array list.
 */
void array_list_sort(struct array_list_t *list, int (*compare_func)(const void *, const void *))
{
  if (!list)
    return;

  qsort(list->items, list->size, sizeof(void *), compare_func);
}
