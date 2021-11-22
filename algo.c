#include <stdio.h>
#include <stdlib.h>

#include "data_structures/priority_queue.h"

static int compare_int(const void *a1, const void *a2)
{
  return *((int *) a1) - *((int *) a2);
}

int main()
{
  struct priority_queue_t *pqueue;
  int a = 1;
  int b = 2;
  int c = 3;
  int d = 4;

  pqueue = priority_queue_create(compare_int);

  priority_queue_push(pqueue, &d);
  priority_queue_push(pqueue, &b);
  priority_queue_push(pqueue, &c);
  priority_queue_push(pqueue, &a);

  printf("%d\n", *((int *) priority_queue_pop(pqueue)));
  printf("%d\n", *((int *) priority_queue_pop(pqueue)));
  printf("%d\n", *((int *) priority_queue_pop(pqueue)));
  printf("%d\n", *((int *) priority_queue_pop(pqueue)));

  priority_queue_free(pqueue);
  return 0;
}
