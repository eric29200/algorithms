#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NB_ELEMENTS   50000

/*
 * Clone an array.
 */
int *clone_array(int *array, int nb_elements)
{
  int *new_array, i;

  if (nb_elements <= 0)
    return NULL;

  new_array = (int *) malloc(sizeof(int) * nb_elements);
  if (!new_array)
    exit(1);

  for (i = 0; i < nb_elements; i++)
    new_array[i] = array[i];

  return new_array;
}

/*
 * Swap two elements.
 */
void swap(int *a, int *b)
{
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

/*
 * Bubble sort :
 * - swap consecutive elements if needed
 * - end when no swap operations are needed
 */
void sort_bubble(int *array, int nb_elements)
{
  int change = 1, i;

  while (change) {
    change = 0;

    for (i = 0; i < nb_elements - 1; i++) {
      if (array[i] > array[i + 1]) {
        swap(&array[i], &array[i + 1]);
        change = 1;
      }
    }
  }
}

/*
 * Insertion sort :
 * - for each position from 0 to n, find minimum element
 */
void sort_insertion(int *array, int nb_elements)
{
  int i, j, min;

  for (i = 0; i < nb_elements; i++) {
    min = i;

    for (j = i + 1; j < nb_elements; j++)
      if (array[j] < array[min])
        min = j;

    if (min != i)
      swap(&array[i], &array[min]);
  }
}

/*
 * Sort an array.
 */
void sort(int *array, int nb_elements, void (*sort_func)(int *, int), char *name)
{
  clock_t start, end;
  int *new_array;

  /* clone array */
  new_array = clone_array(array, NB_ELEMENTS);
  if (!new_array)
    return;

  /* sort */
  start = clock();
  sort_func(new_array, nb_elements);
  end = clock();

  /* free array */
  free(new_array);

  /* print stats */
  printf("%s : %f\n", name, (end - start) / (double) CLOCKS_PER_SEC);
}

int main()
{
  int array[NB_ELEMENTS], i;

  /* create array */
  for (i = 0; i < NB_ELEMENTS; i++)
    array[i] = rand() % 100;

  /* bubble sort */
  sort(array, NB_ELEMENTS, sort_bubble, "Bubble sort");

  /* insertion sort */
  sort(array, NB_ELEMENTS, sort_insertion, "Insertion sort");

  return 0;
}
