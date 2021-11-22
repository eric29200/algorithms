#ifndef _PRIORITY_QUEUE_H_
#define _PRIORITY_QUEUE_H_

#include <stdio.h>

#include "heap.h"

struct priority_queue_t {
  struct heap_t *heap;
  int (*compare_func)(const void *, const void *);
};

struct priority_queue_t *priority_queue_create(int capacity, int (*compare_func)(const void *, const void *));
void priority_queue_free(struct priority_queue_t *pqueue);
void priority_queue_free_full(struct priority_queue_t *pqueue, void (*free_func)(void *));
int priority_queue_is_empty(struct priority_queue_t *pqueue);
int priority_queue_is_full(struct priority_queue_t *pqueue);
void priority_queue_push(struct priority_queue_t *pqueue, void *data);
void *priority_queue_peek(struct priority_queue_t *pqueue);
void *priority_queue_pop(struct priority_queue_t *pqueue);

#endif
