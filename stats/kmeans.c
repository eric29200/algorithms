#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "kmeans.h"
#include "../utils/mem.h"

/*
 * Create a cluster.
 */
static struct cluster_t *cluster_create(void *centroid, size_t item_size)
{
  struct cluster_t *cluster;

  cluster = (struct cluster_t *) xmalloc(sizeof(struct cluster_t));
  cluster->centroid = xmalloc(item_size);
  memcpy(cluster->centroid, centroid, item_size);
  cluster->items = NULL;
  cluster->nb_items = 0;

  return cluster;
}

/*
 * Free a cluster.
 */
void cluster_free(struct cluster_t *cluster)
{
  if (!cluster)
    return;

  xfree(cluster->centroid);
  xfree(cluster->items);
  free(cluster);
}

/*
 * Assign items to clusters.
 */
static size_t compute_items(void *items, size_t nb_items, size_t item_size,
                               struct cluster_t **clusters, size_t nb_clusters,
                               size_t *items2clusters,
                               double (*distance_func)(const void *, const void *))
{
  size_t i, j, cluster_min, ret = 0;
  double dist, dist_min;
  void *item;

  /* foreach item */
  for (i = 0; i < nb_items; i++) {
    item = items + i * item_size;

    /* compute nearest cluster */
    for (j = 0, cluster_min = -1; j < nb_clusters; j++) {
      dist = distance_func(item, clusters[j]->centroid);
      if (j == 0 || dist < dist_min) {
        cluster_min = j;
        dist_min = dist;
      }
    }

    /* update item2cluster */
    if (cluster_min != items2clusters[i]) {
      items2clusters[i] = cluster_min;
      ret++;
    }
  }

  return ret;
}

/*
 * Compute clusters.
 */
static void compute_clusters(void *items, size_t nb_items, size_t item_size,
                             struct cluster_t **clusters, size_t nb_clusters,
                             size_t *items2clusters,
                             void (*mean_func)(void *, size_t, void *))
{
  struct cluster_t *cluster;
  size_t i;

  /* clear clusters */
  for (i = 0; i < nb_clusters; i++) {
    xfree(clusters[i]->items);
    clusters[i]->nb_items = 0;
  }

  /* get number of items by cluster */
  for (i = 0; i < nb_items; i++) {
    cluster = clusters[items2clusters[i]];
    cluster->nb_items++;
  }

  /* allocate cluster items */
  for (i = 0; i < nb_clusters; i++) {
    clusters[i]->items = xmalloc(clusters[i]->nb_items * item_size);
    clusters[i]->nb_items = 0;
  }

  /* put items in clusters */
  for (i = 0; i < nb_items; i++) {
    cluster = clusters[items2clusters[i]];
    memcpy(cluster->items + cluster->nb_items * item_size, items + i * item_size, item_size);
    cluster->nb_items++;
  }

  /* recompute centroids */
  for (i = 0; i < nb_clusters; i++)
    mean_func(clusters[i]->items, clusters[i]->nb_items, clusters[i]->centroid);
}

/*
 * Kmeans algorithm.
 */
struct cluster_t **kmeans(void *items, size_t nb_items, size_t item_size, size_t k,
                          double (*distance_func)(const void *, const void *),
                          void (*mean_func)(void *, size_t, void *))
{
  struct cluster_t **clusters;
  size_t *items2clusters;
  size_t i, ret;

  /* check K */
  if (nb_items < k)
    return NULL;

  /* create clusters */
  clusters = (struct cluster_t **) xmalloc(sizeof(struct cluster_t *) * k);
  for (i = 0; i < k; i++)
    clusters[i] = cluster_create(items + (rand() / nb_items) * item_size, item_size);

  /* create items2clusters array */
  items2clusters = (size_t *) xmalloc(sizeof(size_t) * nb_items);
  for (i = 0; i < nb_items; i++)
    items2clusters[i] = -1;

  /* compute */
  for (;;) {
    /* assign items to clusters */
    ret = compute_items(items, nb_items, item_size, clusters, k,
                           items2clusters, distance_func);

    /* compute clusters */
    compute_clusters(items, nb_items, item_size, clusters, k, items2clusters,
                     mean_func);

    /* no more changes : break */
    if (ret <= 0)
      break;
  }

  /* free items2clusters array */
  free(items2clusters);

  return clusters;
}
