#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "list.h"

struct queue_t {
  struct list_t *head;
  struct list_t *tail;
  size_t length;
};

struct queue_t *queue_create();
void queue_free(struct queue_t *queue);
int queue_is_empty(struct queue_t *queue);
struct queue_t *queue_copy(struct queue_t *queue);
struct list_t *queue_find(struct queue_t *queue, void *data);
void queue_push_head(struct queue_t *queue, void *data);
void queue_push_tail(struct queue_t *queue, void *data);
void *queue_pop_head(struct queue_t *queue);
void *queue_pop_tail(struct queue_t *queue);
void *queue_peek_head(struct queue_t *queue);
void *queue_peek_tail(struct queue_t *queue);

#endif
