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
  list->elements = (void **) xrealloc(list->elements, sizeof(void *) * list->capacity);
}

/*
 * Create an array list.
 */
struct array_list_t *array_list_create()
{
  struct array_list_t *list;

  list = (struct array_list_t *) xmalloc(sizeof(struct array_list_t));
  list->elements = NULL;
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

  xfree(list->elements);
  free(list);
}

/*
 * Free an array list and its elements.
 */
void array_list_free_full(struct array_list_t *list, void (*free_func)(void *))
{
  size_t i;

  if (!list)
    return;

  if (list->elements) {
    for (i = 0; i < list->size; i++)
      free_func(list->elements[i]);

    free(list->elements);
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

  xfree(list->elements);
  list->elements = NULL;
  list->size = 0;
  list->capacity = 0;
}

/*
 * Add an element at the end of a list.
 */
void array_list_add(struct array_list_t *list, void *element)
{
  if (!list)
    return;

  /* grow array list if needed */
  __array_list_grow(list);

  /* add element */
  list->elements[list->size++] = element;
}

/*
 * Add an element at index (shift other elements on the right).
 */
void array_list_add_idx(struct array_list_t *list, size_t idx, void *element)
{
  size_t i;

  if (!list || idx < 0 || idx > list->size)
    return;

  /* grow array list if needed */
  __array_list_grow(list);

  /* shif elements on the right */
  for (i = list->size; i > idx; i--)
    list->elements[i] = list->elements[i - 1];

  /* add element */
  list->elements[idx] = element;
  list->size++;
}

/*
 * Set an element in a list.
 */
void *array_list_set(struct array_list_t *list, size_t idx, void *element)
{
  void *ret;

  if (!list || idx < 0 || idx >= list->size)
    return NULL;

  ret = list->elements[idx];
  list->elements[idx] = element;
  return ret;
}

/*
 * Remove an element in a list.
 */
void *array_list_remove(struct array_list_t *list, size_t idx)
{
  void *ret;
  size_t i;

  if (!list || idx < 0 || idx >= list->size)
    return NULL;

  /* save element */
  ret = list->elements[idx];

  /* shift elements on the left */
  for (i = idx; i < list->size - 1; i++)
    list->elements[i] = list->elements[i + 1];

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

  qsort(list->elements, list->size, sizeof(void *), compare_func);
}
