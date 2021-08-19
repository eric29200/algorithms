#include <stdlib.h>

#include "stack.h"

/*
 * Create a stack.
 */
struct stack_t *stack_create(size_t capacity)
{
  struct stack_t *stack;
  size_t i;

  stack = (struct stack_t *) malloc(sizeof(struct stack_t));
  if (!stack)
    return NULL;

  stack->array = (void **) malloc(sizeof(void *) * capacity);
  if (!stack->array) {
    free(stack);
    return NULL;
  }

  stack->top = -1;
  stack->capacity = capacity;
  for (i = 0; i < stack->capacity; i++)
    stack->array[i] = NULL;

  return stack;
}

/*
 * Free a stack.
 */
void stack_free(struct stack_t *stack)
{
  if (stack) {
    if (stack->array)
      free(stack->array);

    free(stack);
  }
}

/*
 * Pop an element from a stack.
 */
void *stack_pop(struct stack_t *stack)
{
  if (stack && !stack_is_empty(stack))
    return stack->array[stack->top--];

  return NULL;
}

/*
 * Peek an element from a stack.
 */
void *stack_peek(struct stack_t *stack)
{
  if (stack && !stack_is_empty(stack))
    return stack->array[stack->top];

  return NULL;
}

/*
 * Push an element on a stack.
 */
void stack_push(struct stack_t *stack, void *data)
{
  if (stack && !stack_is_full(stack))
    stack->array[++stack->top] = data;
}
