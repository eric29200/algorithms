#ifndef _PLOT_H_
#define _PLOT_H_

#include <stdio.h>

#include "../geometry/geometry.h"

#define DATASET_FILE_PATTERN      "/tmp/gnuplot-XXXXXX"
#define DATASET_FILE_LEN          64

/* plot structure */
struct plot_t {
  FILE *fp;
  char **datasets;
  size_t nb_datasets;
};

void plot_init();
void plot_close();
int plot_add_dataset(struct point_t *points, size_t nb_points);
void plot_print();

#endif
