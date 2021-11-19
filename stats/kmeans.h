#ifndef _KMEANS_H_
#define _KMEANS_H_

#include <stdio.h>

#include "../utils/point.h"

struct cluster_t {
  struct point_t centroid;
  struct point_t *points;
  size_t nb_points;
};

void cluster_free(struct cluster_t *cluster);
struct cluster_t **kmeans(struct point_t *points, size_t nb_points, size_t k);

#endif
