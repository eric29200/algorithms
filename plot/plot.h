#ifndef _PLOT_H_
#define _PLOT_H_

#include <stdio.h>

#define DATASET_FILE_PATTERN      "/tmp/gnuplot-XXXXXX"
#define DATASET_FILE_LEN          64

/* plot structure */
struct plot_t {
  FILE *fp;
  char dataset[DATASET_FILE_LEN];
};

void plot_init();
int plot_dataset(double *x, double *y, size_t nb_points);

#endif
