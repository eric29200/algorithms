#include <stdio.h>
#include <stdlib.h>

#include "plot/plot.h"
#include "stats/kmeans.h"

#define NB_POINTS     10000
#define K             4

/*
 * Main.
 */
int main(int argc, char **argv)
{
  struct point_t points[NB_POINTS];
  struct cluster_t **clusters;
  size_t i;

  /* init plot */
  plot_init();

  /* create dataset */
  for (i = 0; i < NB_POINTS; i++) {
    points[i].x = (double) rand() / RAND_MAX;
    points[i].y = (double) rand() / RAND_MAX;
    points[i].cluster_id = -1;
  }

  /* kmeans */
  clusters = kmeans(points, NB_POINTS, K);

  /* plot dataset */
  plot_add_dataset(clusters[0]->points, clusters[0]->nb_points);
  plot_add_dataset(clusters[1]->points, clusters[1]->nb_points);
  plot_add_dataset(clusters[2]->points, clusters[2]->nb_points);
  plot_add_dataset(clusters[3]->points, clusters[3]->nb_points);

  /* print gnuplot */
  plot_print();

  /* close plot */
  plot_close();

  return 0;
}
