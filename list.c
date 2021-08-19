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
 * Append an element at the end of a list.
 */
struct list_t *list_append(struct list_t *list, void *data)
{
  struct list_t *new_list;

  /* allocate new element */
  new_list = (struct list_t *) malloc(sizeof(struct list_t));
  if (!new_list)
    return NULL;

  /* set new element */
  new_list->data = data;
  new_list->next = NULL;

  /* append to the list */
  if (list)
    list_last(list)->next = new_list;
  else
    list = new_list;

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

  return new_list;
}

/*
 * Concatenate 2 lists.
 */
struct list_t *list_concat(struct list_t *list1, struct list_t *list2)
{
  if (!list2)
    return list1;

  if (list1) {
    list_last(list1)->next = list2;
    return list1;
  }

  return list2;
}

/*
 * Remove an element from a list (only remove first matching element).
 */
struct list_t *list_remove(struct list_t *list, void *data)
{
  struct list_t *prev = NULL, *tmp;

  tmp = list;
  while (tmp) {
    if (tmp->data == data) {
      if (prev)
        prev->next = tmp->next;
      else
        list = tmp->next;

      free(tmp);
      break;
    }

    prev = tmp;
    tmp = tmp->next;
  }

  return list;
}

/*
 * Remove all elements from a list.
 */
struct list_t *list_remove_all(struct list_t *list, void *data)
{
  struct list_t *prev = NULL, *next, *tmp;

  tmp = list;
  while (tmp) {
    if (tmp->data == data) {
      next = tmp->next;

      if (prev)
        prev->next = next;
      else
        list = next;

      free(tmp);
      tmp = next;
    } else {
      prev = tmp;
      tmp = tmp->next;
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
