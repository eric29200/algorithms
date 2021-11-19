#include <stdio.h>
#include <stdlib.h>

#include "plot/plot.h"

/*
 * Main.
 */
int main(int argc, char **argv)
{
  double x[10], y[10];
  size_t i;

  /* init plot */
  plot_init();

  /* create dataset */
  for (i = 0; i < 10; i++) {
    x[i] = (double) rand() / RAND_MAX;
    y[i] = (double) rand() / RAND_MAX;
  }

  /* plot dataset */
  plot_dataset(x, y, 10);

  return 0;
}
