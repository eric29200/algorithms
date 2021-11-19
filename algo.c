#include <stdio.h>
#include <stdlib.h>

#include "plot/plot.h"
#include "stats/kmeans.h"
#include "utils/mem.h"

#define NB_POINTS     100000
#define K             50

/*
 * Compute distance between 2 points.
 */
static double distance(const void *a1, const void *a2)
{
  struct point_t *p1 = (struct point_t *) a1;
  struct point_t *p2 = (struct point_t *) a2;

  return (p1->x - p2->x) * (p1->x - p2->x) + (p1->y - p2->y) * (p1->y - p2->y);
}

/*
 * Compute mean.
 */
static void mean(void *elements, size_t nb_elements, void *res)
{
  struct point_t *points = (struct point_t *) elements;
  struct point_t *point_res = (struct point_t *) res;
  size_t i;

  /* reset mean */
  point_res->x = 0;
  point_res->y = 0;

  /* no points */
  if (nb_elements == 0)
    return;

  /* sum x/y */
  for (i = 0; i < nb_elements; i++) {
    point_res->x += points[i].x;
    point_res->y += points[i].y;
  }

  /* compute mean */
  point_res->x /= nb_elements;
  point_res->y /= nb_elements;
}

/*
 * Main.
 */
int main()
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
  clusters = kmeans(points, NB_POINTS, sizeof(struct point_t), K,
                    distance, mean);

  /* plot dataset */
  for (i = 0; i < K; i++)
    plot_add_dataset(clusters[i]->elements, clusters[i]->nb_elements);

  /* print gnuplot */
  plot_print();

  /* close plot */
  plot_close();

  /* free clusters */
  if (clusters) {
    for (i = 0; i < K; i++)
      cluster_free(clusters[i]);

    free(clusters);
  }

  return 0;
}
