#include <stdio.h>
#include <stdlib.h>

#include "plot/plot.h"
#include "stats/kmeans.h"
#include "utils/mem.h"

#define NB_POINTS     100000
#define K             100
#define NB_THREADS    8

/*
 * Main.
 */
int main(int argc, char **argv)
{
  struct cluster_t **clusters;
  struct point_t *points;
  size_t i;

  /* init plot */
  plot_init();

  /* create dataset */
  points = (struct point_t *) xmalloc(sizeof(struct point_t) * NB_POINTS);
  for (i = 0; i < NB_POINTS; i++) {
    points[i].x = (double) rand() / RAND_MAX;
    points[i].y = (double) rand() / RAND_MAX;
    points[i].cluster_id = -1;
  }

  /* kmeans */
  clusters = kmeans(points, NB_POINTS, K, NB_THREADS);

  /* plot dataset */
  for (i = 0; i < K; i++)
    plot_add_dataset(clusters[i]->points, clusters[i]->nb_points);

  /* print gnuplot */
  plot_print();

  /* close plot */
  plot_close();

  return 0;
}
