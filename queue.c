#include <stdlib.h>

#include "queue.h"

/*
 * Create a queue.
 */
struct queue_t *queue_create(size_t capacity)
{
  struct queue_t *queue;
  size_t i;

  queue = (struct queue_t *) malloc(sizeof(struct queue_t));
  if (!queue)
    return NULL;

  queue->array = (void **) malloc(sizeof(void *) * capacity);
  if (!queue->array) {
    free(queue);
    return NULL;
  }

  queue->capacity = capacity;
  queue->size = 0;
  queue->front = 0;
  queue->back = 0;
  for (i = 0; i < queue->capacity; i++)
    queue->array[i] = NULL;

  return queue;
}

/*
 * Free a queue.
 */
void queue_free(struct queue_t *queue)
{
  if (queue) {
    if (queue->array)
      free(queue->array);

    free(queue);
  }
}

/*
 * Enqueue an element.
 */
void queue_enqueue(struct queue_t *queue, void *data)
{
  if (queue && !queue_is_full(queue)) {
    queue->array[queue->back] = data;
    queue->back = (queue->back + 1) % queue->capacity;
    queue->size++;
  }
}

/*
 * Dequeue an element.
 */
void *queue_dequeue(struct queue_t *queue)
{
  void *ret = NULL;

  if (queue && !queue_is_empty(queue)) {
    ret = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
  }

  return ret;
}
