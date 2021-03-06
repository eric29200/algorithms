#include <stdlib.h>

#include "list.h"
#include "../utils/mem.h"

/*
 * Free all items of a list.
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
 * Free all items of a list.
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
 * Append an item at the end of a list.
 */
struct list_t *list_append(struct list_t *list, void *data)
{
  struct list_t *new_list, *last;

  /* allocate new item */
  new_list = xmalloc(sizeof(struct list_t));

  /* set new item */
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
 * Prepend an item at the beginning of a list.
 */
struct list_t *list_prepend(struct list_t *list, void *data)
{
  struct list_t *new_list;

  /* allocate new item */
  new_list = (struct list_t *) xmalloc(sizeof(struct list_t));

  /* set new item */
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
 * Insert an item after position.
 */
struct list_t *list_insert_after(struct list_t *list, struct list_t *pos, void *data)
{
  struct list_t *new_item;

  if (!list || !pos)
    return list;

  /* allocate new item */
  new_item = (struct list_t *) xmalloc(sizeof(struct list_t));
  new_item->data = data;
  new_item->prev = pos;
  new_item->next = pos->next;
  pos->next = new_item;

  return list;
}

/*
 * Insert an item before position.
 */
struct list_t *list_insert_before(struct list_t *list, struct list_t *pos, void *data)
{
  struct list_t *new_item;

  if (!list || !pos)
    return list;

  /* allocate new item */
  new_item = (struct list_t *) xmalloc(sizeof(struct list_t));
  new_item->data = data;
  new_item->prev = pos->prev;
  new_item->next = pos;
  pos->prev = new_item;

  /* update list */
  if (!new_item->prev)
    list = new_item;

  return list;
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
 * Remove an item from a list (only remove first matching item).
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
 * Remove all items from a list.
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
    new_elt = (struct list_t *) xmalloc(sizeof(struct list_t));
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
 * Find an item in a list.
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
 * Get the first item of a list.
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
 * Get the last item of a list.
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
 * Get middle item of a list.
 */
struct list_t *list_middle(struct list_t *list)
{
  struct list_t *l1, *l2;

  if (!list)
    return NULL;

  /* l2 moves forward 2 times faster than l1 */
  for (l1 = list, l2 = list->next; l2 != NULL;) {
    l2 = l2->next;
    if (l2) {
        l1 = l1->next;
        l2 = l2->next;
      }
  }

  return l1;
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
 * Merge 2 sorted lists.
 */
static struct list_t *__list_sort_merge(struct list_t *l1, struct list_t *l2, int (*compare_func)(const void *, const void *))
{
  struct list_t *res;
  int cmp;

  if (!l1)
    return l2;
  if (!l2)
    return l1;

  cmp = compare_func(l1->data, l2->data);
  if (cmp <= 0) {
    res = l1;
    res->next = __list_sort_merge(l1->next, l2, compare_func);
  } else {
    res = l2;
    res->next = __list_sort_merge(l1, l2->next, compare_func);
  }

  return res;
}

/*
 * Sort a list (merge sort).
 */
struct list_t *list_sort(struct list_t *list, int (*compare_func)(const void *, const void *))
{
  struct list_t *l1, *l2;

  if (!list || !list->next)
    return list;

  /* get middle element */
  l1 = list_middle(list);

  /* l1 is just before the middle item */
  l2 = l1->next;
  l1->next = NULL;
  l1 = list;

  /* recursive merge sort */
  l1 = list_sort(l1, compare_func);
  l2 = list_sort(l2, compare_func);

  /* merge sorted lists */
  return __list_sort_merge(l1, l2, compare_func);
}
