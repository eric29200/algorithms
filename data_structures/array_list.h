#ifndef _ARRAY_LIST_H_
#define _ARRAY_LIST_H_

#include <stdio.h>

struct array_list_t {
  void **elements;
  size_t size;
  size_t capacity;
};

struct array_list_t *array_list_create();
void array_list_free(struct array_list_t *list);
void array_list_free_full(struct array_list_t *list, void (*free_func)(void *));
void array_list_add(struct array_list_t *list, void *element);
void array_list_add_idx(struct array_list_t *list, size_t idx, void *element);
void *array_list_set(struct array_list_t *list, size_t idx, void *element);
void *array_list_remove(struct array_list_t *list, size_t idx);
void array_list_sort(struct array_list_t *list, int (*compare_func)(const void *, const void *));

#endif
