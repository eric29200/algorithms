#ifndef _STACK_H_
#define _STACK_H_

#include <stdio.h>

struct stack_t {
  void **array;
  size_t top;
  size_t capacity;
};

#define stack_is_empty(stack)           ((stack) != NULL && (stack)->top == -1)
#define stack_is_full(stack)            ((stack) != NULL && (stack)->top == (stack)->capacity - 1)

struct stack_t *stack_create(size_t capacity);
void *stack_pop(struct stack_t *stack);
void *stack_peek(struct stack_t *stack);
void stack_push(struct stack_t *stack, void *data);

#endif
