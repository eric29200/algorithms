#include <stdlib.h>

#include "queue.h"
#include "../utils/mem.h"

/*
 * Create a queue.
 */
struct queue_t *queue_create()
{
  struct queue_t *queue;

  queue = (struct queue_t *) xmalloc(sizeof(struct queue_t));
  queue->head = NULL;
  queue->tail = NULL;
  queue->length = 0;

  return queue;
}

/*
 * Free a queue.
 */
void queue_free(struct queue_t *queue)
{
  if (!queue)
    return;

  list_free(queue->head);
  free(queue);
}

/*
 * Free a queue.
 */
void queue_free_full(struct queue_t *queue, void (*free_func)(void *))
{
  if (!queue)
    return;

  list_free_full(queue->head, free_func);
  free(queue);
}

/*
 * Is queue empty ?.
 */
int queue_is_empty(struct queue_t *queue)
{
  return queue->head == NULL;
}

/*
 * Copy a queue.
 */
struct queue_t *queue_copy(struct queue_t *queue)
{
  struct queue_t *res;
  struct list_t *list;

  if (!queue)
    return NULL;

  res = queue_create();
  if (!res)
    return NULL;

  for (list = queue->head; list != NULL; list = list->next)
    queue_push_tail(res, list->data);

  return res;
}

/*
 * Find an element in a queue.
 */
struct list_t *queue_find(struct queue_t *queue, void *data)
{
  if (!queue)
    return NULL;

  return list_find(queue->head, data);
}

/*
 * Push an element at the head of a queue.
 */
void queue_push_head(struct queue_t *queue, void *data)
{
  if (!queue)
    return;

  queue->head = list_prepend(queue->head, data);
  if (!queue->tail)
    queue->tail = queue->head;

  queue->length++;
}

/*
 * Push an element at the tail of a queue.
 */
void queue_push_tail(struct queue_t *queue, void *data)
{
  if (!queue)
    return;

  queue->tail = list_append(queue->tail, data);
  if (queue->tail->next)
    queue->tail = queue->tail->next;
  else
    queue->head = queue->tail;

  queue->length++;
}

/*
 * Pop an element from the head of a queue.
 */
void *queue_pop_head(struct queue_t *queue)
{
  struct list_t *list;
  void *data;

  if (!queue || !queue->head)
    return NULL;

  list = queue->head;
  data = list->data;

  queue->head = list->next;
  if (queue->head)
    queue->head->prev = NULL;
  else
    queue->tail = NULL;

  free(list);
  queue->length--;

  return data;
}

/*
 * Pop an element from the tail of a queue.
 */
void *queue_pop_tail(struct queue_t *queue)
{
  struct list_t *list;
  void *data;

  if (!queue || !queue->tail)
    return NULL;

  list = queue->tail;
  data = list->data;

  queue->tail = list->prev;
  if (queue->tail)
    queue->tail->next = NULL;
  else
    queue->head = NULL;

  return data;
}

/*
 * Peek the head of a queue.
 */
void *queue_peek_head(struct queue_t *queue)
{
  if (!queue || !queue->head)
    return NULL;

  return queue->head->data;
}

/*
 * Peek the tail of a queue.
 */
void *queue_peek_tail(struct queue_t *queue)
{
  if (!queue || !queue->tail)
    return NULL;

  return queue->tail->data;
}
