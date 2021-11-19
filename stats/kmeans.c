#include <stdio.h>
#include <stdlib.h>

#include "kmeans.h"
#include "../utils/mem.h"

#define DISTANCE(p1, p2)      (((p1).x - (p2).x) * ((p1).x - (p2).x) + ((p1).y - (p2).y) * ((p1).y - (p2).y))

/*
 * Create a cluster.
 */
static struct cluster_t *cluster_create(struct point_t centroid)
{
  struct cluster_t *cluster;

  cluster = (struct cluster_t *) xmalloc(sizeof(struct cluster_t));
  cluster->centroid = centroid;
  cluster->points = NULL;
  cluster->nb_points = 0;

  return cluster;
}

/*
 * Free a cluster.
 */
void cluster_free(struct cluster_t *cluster)
{
  if (!cluster)
    return;

  xfree(cluster->points);
  free(cluster);
}

/*
 * Assign points to clusters.
 */
static size_t points2clusters(struct point_t *points, size_t nb_points, struct cluster_t **clusters, size_t nb_clusters)
{
  size_t i, j, cluster_min;
  double dist, dist_min;
  size_t ret;

  /* reset number of changes */
  ret = 0;

  /* assign points to clusters */
  for (i = 0; i < nb_points; i++) {
    cluster_min = -1;

    for (j = 0; j < nb_clusters; j++) {
      dist = DISTANCE(points[i], clusters[j]->centroid);
      if (j == 0 || dist < dist_min) {
        dist_min = dist;
        cluster_min = j;
      }
    }

    /* reassign point */
    if (cluster_min != points[i].cluster_id) {
      points[i].cluster_id = cluster_min;
      ret++;
    }
  }

  return ret;
}

/*
 * Compute cluster centroid.
 */
static void clusters_compute_centroid(struct point_t *points, size_t nb_points,
                                      struct cluster_t **clusters, size_t nb_clusters)
{
  size_t i;

  /* reset clusters */
  for (i = 0; i < nb_clusters; i++) {
    clusters[i]->centroid.x = 0;
    clusters[i]->centroid.y = 0;
    clusters[i]->nb_points = 0;
  }

  /* sum x/y */
  for (i = 0; i < nb_points; i++) {
    clusters[points[i].cluster_id]->centroid.x += points[i].x;
    clusters[points[i].cluster_id]->centroid.y += points[i].y;
    clusters[points[i].cluster_id]->nb_points++;
  }

  /* compute mean */
  for (i = 0; i < nb_clusters; i++) {
    if (clusters[i]->nb_points > 0) {
      clusters[i]->centroid.x /= clusters[i]->nb_points;
      clusters[i]->centroid.y /= clusters[i]->nb_points;
    }
  }
}

/*
 * Put points in clusters.
 */
static void clusters_put_points(struct point_t *points, size_t nb_points, struct cluster_t **clusters, size_t nb_clusters)
{
  struct cluster_t *cluster;
  size_t i;

  /* allocate clusters points */
  for (i = 0; i < nb_clusters; i++) {
    if (clusters[i]->nb_points <= 0)
      continue;

    clusters[i]->points = (struct point_t *) xmalloc(sizeof(struct point_t) * clusters[i]->nb_points);
    clusters[i]->nb_points = 0;
  }

  /* put points in clusters */
  for (i = 0; i < nb_points; i++) {
    cluster = clusters[points[i].cluster_id];
    cluster->points[cluster->nb_points++] = points[i];
  }
}

/*
 * Kmeans algorithm.
 */
struct cluster_t **kmeans(struct point_t *points, size_t nb_points, size_t k)
{
  struct cluster_t **clusters;
  size_t i;
  int ret;

  /* check K */
  if (nb_points < k)
    return NULL;

  /* create clusters */
  clusters = (struct cluster_t **) xmalloc(sizeof(struct cluster_t) * k);
  for (i = 0; i < k; i++)
    clusters[i] = cluster_create(points[rand() % nb_points]);

  /* reset clusters ids */
  for (i = 0; i < nb_points; i++)
    points[i].cluster_id = -1;

  /* compute clusters */
  for (;;) {
    /* assign points to clusters */
    ret = points2clusters(points, nb_points, clusters, k);

    /* recompute clusters centroids */
    clusters_compute_centroid(points, nb_points, clusters, k);

    /* no more changes : exit */
    if (ret == 0)
      break;
  }

  /* finally put points in clusters */
  clusters_put_points(points, nb_points, clusters, k);

  return clusters;
}
