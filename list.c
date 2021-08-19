#include <stdlib.h>

#include "list.h"

/*
 * Free all elements of a list.
 */
void list_free(struct list_t *list)
{
  struct list_t *next;

  while (list) {
    next = list->next;
    free(list);
    list = next;
  }
}

/*
 * Free all elements of a list.
 */
void list_free_full(struct list_t *list, void (*free_func)(void *))
{
  struct list_t *next;

  while (list) {
    next = list->next;
    free_func(list->data);
    free(list);
    list = next;
  }
}

/*
 * Append an element at the end of a list.
 */
struct list_t *list_append(struct list_t *list, void *data)
{
  struct list_t *new_list, *last;

  /* allocate new element */
  new_list = (struct list_t *) malloc(sizeof(struct list_t));
  if (!new_list)
    return NULL;

  /* set new element */
  new_list->data = data;
  new_list->next = NULL;

  /* append to the list */
  if (list) {
    last = list_last(list);
    last->next = new_list;
    new_list->prev = last;
  } else {
    new_list->prev = NULL;
    list = new_list;
  }

  return list;
}

/*
 * Prepend an element at the beginning of a list.
 */
struct list_t *list_prepend(struct list_t *list, void *data)
{
  struct list_t *new_list;

  /* allocate new element */
  new_list = (struct list_t *) malloc(sizeof(struct list_t));
  if (!new_list)
    return NULL;

  /* set new element */
  new_list->data = data;
  new_list->next = list;

  if (list) {
    new_list->prev = list->prev;
    if (list->prev)
      list->prev->next = new_list;
    list->prev = new_list;
  } else {
    new_list->prev = NULL;
  }

  return new_list;
}

/*
 * Concatenate 2 lists.
 */
struct list_t *list_concat(struct list_t *list1, struct list_t *list2)
{
  struct list_t *last;

  if (!list2)
    return list1;

  last = list_last(list1);
  if (last)
    last->next = list2;
  else
    list1 = list2;
  list2->prev = last;

  return list1;
}

/*
 * Remove an element from a list (only remove first matching element).
 */
struct list_t *list_remove(struct list_t *list, void *data)
{
  struct list_t *next, *tmp;

  tmp = list;
  while (tmp) {
    if (tmp->data != data) {
      tmp = tmp->next;
    } else {
      next = tmp->next;

      if (tmp->prev)
        tmp->prev->next = next;
      else
        list = next;

      if (next)
        next->prev = tmp->prev;

      free(tmp);
      tmp = next;
      break;
    }
  }

  return list;
}

/*
 * Remove all elements from a list.
 */
struct list_t *list_remove_all(struct list_t *list, void *data)
{
  struct list_t *next, *tmp;

  tmp = list;
  while (tmp) {
    if (tmp->data != data) {
      tmp = tmp->next;
    } else {
      next = tmp->next;

      if (tmp->prev)
        tmp->prev->next = next;
      else
        list = next;

      if (next)
        next->prev = tmp->prev;

      free(tmp);
      tmp = next;
    }
  }

  return list;
}

/*
 * Copy a list.
 */
struct list_t *list_copy(struct list_t *list)
{
  struct list_t *new_list = NULL, *new_elt, *prev = NULL;

  for (; list != NULL; list = list->next) {
    new_elt = (struct list_t *) malloc(sizeof(struct list_t));
    if (!new_elt) {
      list_free(new_list);
      return NULL;
    }

    new_elt->data = list->data;
    new_elt->prev = prev;
    if (prev)
      prev->next = new_elt;
    else
      new_list = new_elt;

    prev = new_elt;
  }

  return new_list;
}

/*
 * Find an element in a list.
 */
struct list_t *list_find(struct list_t *list, void *data)
{
  while (list) {
    if (list->data == data)
      return list;

    list = list->next;
  }

  return NULL;
}

/*
 * Get the first element of a list.
 */
struct list_t *list_first(struct list_t *list)
{
  if (!list)
    return NULL;

  while (list->prev)
    list = list->prev;

  return list;
}

/*
 * Get the last element of a list.
 */
struct list_t *list_last(struct list_t *list)
{
  if (!list)
    return NULL;

  while (list->next)
    list = list->next;

  return list;
}

/*
 * Get the length of a list.
 */
size_t list_length(struct list_t *list)
{
  size_t len = 0;

  while (list) {
    len++;
    list = list->next;
  }

  return len;
}

/*
 * Swap 2 elements of a list.
 */
static void list_swap(struct list_t *l1, struct list_t *l2)
{
  void *tmp = l1->data;
  l1->data = l2->data;
  l2->data = tmp;
}

/*
 * Sort a list (bubble sort).
 */
void list_sort_bubble(struct list_t *list, int (*compare_func)(const void *, const void *))
{
  struct list_t *node;
  int change = 1;

  if (!list || !list->next)
    return;

  while (change) {
    change = 0;

    for (node = list->next; node != NULL; node = node->next) {
      if (compare_func(node->data, node->prev->data) < 0) {
        list_swap(node, node->prev);
        change = 1;
      }
    }
  }
}

/*
 * Sort a list (insertion sort).
 */
void list_sort_insertion(struct list_t *list, int (*compare_func)(const void *, const void *))
{
  struct list_t *i, *j, *min;

  if (!list || !list->next)
    return;

  for (i = list; i != NULL; i = i->next) {
    min = i;

    for (j = i->next; j != NULL; j = j->next)
      if (compare_func(j->data, min->data) < 0)
        min = j;

    if (min != i)
      list_swap(i, min);
  }
}
