#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdio.h>

struct queue_t {
  void **array;
  size_t capacity;
  size_t size;
  size_t front;
  size_t back;
};

#define queue_is_empty(queue)     ((queue) != NULL && (queue)->size == 0)
#define queue_is_full(queue)      ((queue) != NULL && (queue)->size == (queue)->capacity)

struct queue_t *queue_create(size_t capacity);
void queue_free(struct queue_t *queue);
void queue_enqueue(struct queue_t *queue, void *data);
void *queue_dequeue(struct queue_t *queue);

#endif
