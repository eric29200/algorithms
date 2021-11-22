#ifndef _SLIST_H_
#define _SLIST_H_

#include <stdio.h>

struct list_t {
  void *data;
  struct list_t *prev;
  struct list_t *next;
};

void list_free(struct list_t *list);
void list_free_full(struct list_t *list, void (*free_func)(void *));
struct list_t *list_append(struct list_t *list, void *data);
struct list_t *list_prepend(struct list_t *list, void *data);
struct list_t *list_insert_after(struct list_t *list, struct list_t *pos, void *data);
struct list_t *list_insert_before(struct list_t *list, struct list_t *pos, void *data);
struct list_t *list_concat(struct list_t *list1, struct list_t *list2);
struct list_t *list_remove(struct list_t *list, void *data);
struct list_t *list_remove_all(struct list_t *list, void *data);
struct list_t *list_copy(struct list_t *list);
struct list_t *list_find(struct list_t *list, void *data);
struct list_t *list_first(struct list_t *list);
struct list_t *list_last(struct list_t *list);
struct list_t *list_middle(struct list_t *list);
size_t list_length(struct list_t *list);
struct list_t *list_sort(struct list_t *list, int (*compare_func)(const void *, const void *));

#endif
