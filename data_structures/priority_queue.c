#include <stdio.h>
#include <stdlib.h>

#include "priority_queue.h"
#include "../utils/mem.h"

/*
 * Create a priority queue.
 */
struct priority_queue_t *priority_queue_create(int (*compare_func)(const void *, const void *))
{
  struct priority_queue_t *pqueue;

  pqueue = (struct priority_queue_t *) xmalloc(sizeof(struct priority_queue_t));
  pqueue->head = NULL;
  pqueue->length = 0;
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

  list_free(pqueue->head);
  free(pqueue);
}

/*
 * Free a priority queue.
 */
void priority_queue_free_full(struct priority_queue_t *pqueue, void (*free_func)(void *))
{
  if (!pqueue)
    return;

  list_free_full(pqueue->head, free_func);
  free(pqueue);
}

/*
 * Check if a priority queue is empty.
 */
int priority_queue_is_empty(struct priority_queue_t *pqueue)
{
  return pqueue && pqueue->head == NULL;
}

/*
 * Push an item in priority queue.
 */
void priority_queue_push(struct priority_queue_t *pqueue, void *data)
{
  struct list_t *it, *prev;

  if (!pqueue)
    return;

  /* find position */
  for (it = pqueue->head, prev = NULL; it != NULL; it = it->next) {
    if (pqueue->compare_func(it->data, data) >= 0)
      break;

    prev = it;
  }

  /* add item */
  if (prev)
    pqueue->head = list_insert_after(pqueue->head, prev, data);
  else
    pqueue->head = list_prepend(pqueue->head, data);
}

/*
 * Peek an item from priority queue.
 */
void *priority_queue_peek(struct priority_queue_t *pqueue)
{
  if (!pqueue || priority_queue_is_empty(pqueue))
      return NULL;

  return pqueue->head->data;
}

/*
 * Pop an item from priority queue.
 */
void *priority_queue_pop(struct priority_queue_t *pqueue)
{
  void *ret;

  if (!pqueue || priority_queue_is_empty(pqueue))
      return NULL;

  /* get first item */
  ret = pqueue->head->data;

  /* remove first item */
  pqueue->head = pqueue->head->next;
  if (pqueue->head)
    pqueue->head->prev = NULL;

  return ret;
}
