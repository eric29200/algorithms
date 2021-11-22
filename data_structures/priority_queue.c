#include <stdio.h>
#include <stdlib.h>

#include "priority_queue.h"
#include "../utils/mem.h"

/*
 * Create a priority queue.
 */
struct priority_queue_t *priority_queue_create(int capacity, int (*compare_func)(const void *, const void *))
{
  struct priority_queue_t *pqueue;

  if (capacity < 0)
    return NULL;

  pqueue = (struct priority_queue_t *) xmalloc(sizeof(struct priority_queue_t));
  pqueue->heap = heap_create(HEAP_MIN, capacity, compare_func);
  pqueue->compare_func = compare_func;

  return pqueue;
}

/*
 * Free a priority queue.
 */
void priority_queue_free(struct priority_queue_t *pqueue)
{
  if (!pqueue)
    return;

  heap_free(pqueue->heap);
  free(pqueue);
}

/*
 * Free a priority queue.
 */
void priority_queue_free_full(struct priority_queue_t *pqueue, void (*free_func)(void *))
{
  if (!pqueue)
    return;

  heap_free_full(pqueue->heap, free_func);
  free(pqueue);
}

/*
 * Check if a priority queue is empty.
 */
int priority_queue_is_empty(struct priority_queue_t *pqueue)
{
  return pqueue && pqueue->heap->size == 0;
}

/*
 * Check if a priority queue is full.
 */
int priority_queue_is_full(struct priority_queue_t *pqueue)
{
  return pqueue && pqueue->heap->size == pqueue->heap->capacity;
}

/*
 * Push an item in priority queue.
 */
void priority_queue_push(struct priority_queue_t *pqueue, void *data)
{
  /* check if queue is not full */
  if (!pqueue || priority_queue_is_full(pqueue))
    return;

  /* insert item in hep */
  heap_insert(pqueue->heap, data);
}

/*
 * Peek an item from priority queue.
 */
void *priority_queue_peek(struct priority_queue_t *pqueue)
{
  if (!pqueue || priority_queue_is_empty(pqueue))
      return NULL;

  /* peek first element of heap */
  return pqueue->heap->data[0];
}

/*
 * Pop an item from priority queue.
 */
void *priority_queue_pop(struct priority_queue_t *pqueue)
{
  if (!pqueue || priority_queue_is_empty(pqueue))
      return NULL;

  /* pop minimum from heap */
  return heap_min(pqueue->heap);
}
