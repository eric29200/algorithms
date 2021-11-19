#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "kmeans.h"
#include "../utils/mem.h"

/*
 * Create a cluster.
 */
static struct cluster_t *cluster_create(void *centroid, size_t element_size)
{
  struct cluster_t *cluster;

  cluster = (struct cluster_t *) xmalloc(sizeof(struct cluster_t));
  cluster->centroid = xmalloc(element_size);
  memcpy(cluster->centroid, centroid, element_size);
  cluster->elements = NULL;
  cluster->nb_elements = 0;

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
  xfree(cluster->elements);
  free(cluster);
}

/*
 * Assign elements to clusters.
 */
static size_t compute_elements(void *elements, size_t nb_elements, size_t element_size,
                               struct cluster_t **clusters, size_t nb_clusters,
                               size_t *elements2clusters,
                               double (*distance_func)(const void *, const void *))
{
  size_t i, j, cluster_min, ret = 0;
  double dist, dist_min;
  void *element;

  /* foreach element */
  for (i = 0; i < nb_elements; i++) {
    element = elements + i * element_size;

    /* compute nearest cluster */
    for (j = 0, cluster_min = -1; j < nb_clusters; j++) {
      dist = distance_func(element, clusters[j]->centroid);
      if (j == 0 || dist < dist_min) {
        cluster_min = j;
        dist_min = dist;
      }
    }

    /* update element2cluster */
    if (cluster_min != elements2clusters[i]) {
      elements2clusters[i] = cluster_min;
      ret++;
    }
  }

  return ret;
}

/*
 * Compute clusters.
 */
static void compute_clusters(void *elements, size_t nb_elements, size_t element_size,
                             struct cluster_t **clusters, size_t nb_clusters,
                             size_t *elements2clusters,
                             void (*mean_func)(void *, size_t, void *))
{
  struct cluster_t *cluster;
  size_t i;

  /* clear clusters */
  for (i = 0; i < nb_clusters; i++) {
    xfree(clusters[i]->elements);
    clusters[i]->nb_elements = 0;
  }

  /* get number of elements by cluster */
  for (i = 0; i < nb_elements; i++) {
    cluster = clusters[elements2clusters[i]];
    cluster->nb_elements++;
  }

  /* allocate cluster elements */
  for (i = 0; i < nb_clusters; i++) {
    clusters[i]->elements = xmalloc(clusters[i]->nb_elements * element_size);
    clusters[i]->nb_elements = 0;
  }

  /* put elements in clusters */
  for (i = 0; i < nb_elements; i++) {
    cluster = clusters[elements2clusters[i]];
    memcpy(cluster->elements + cluster->nb_elements * element_size, elements + i * element_size, element_size);
    cluster->nb_elements++;
  }

  /* recompute centroids */
  for (i = 0; i < nb_clusters; i++)
    mean_func(clusters[i]->elements, clusters[i]->nb_elements, clusters[i]->centroid);
}

/*
 * Kmeans algorithm.
 */
struct cluster_t **kmeans(void *elements, size_t nb_elements, size_t element_size, size_t k,
                          double (*distance_func)(const void *, const void *),
                          void (*mean_func)(void *, size_t, void *))
{
  struct cluster_t **clusters;
  size_t *elements2clusters;
  size_t i, ret;

  /* check K */
  if (nb_elements < k)
    return NULL;

  /* create clusters */
  clusters = (struct cluster_t **) xmalloc(sizeof(struct cluster_t *) * k);
  for (i = 0; i < k; i++)
    clusters[i] = cluster_create(elements + (rand() / nb_elements) * element_size, element_size);

  /* create elements2clusters array */
  elements2clusters = (size_t *) xmalloc(sizeof(size_t) * nb_elements);
  for (i = 0; i < nb_elements; i++)
    elements2clusters[i] = -1;

  /* compute */
  for (;;) {
    /* assign elements to clusters */
    ret = compute_elements(elements, nb_elements, element_size, clusters, k,
                           elements2clusters, distance_func);

    /* compute clusters */
    compute_clusters(elements, nb_elements, element_size, clusters, k, elements2clusters,
                     mean_func);

    /* no more changes : break */
    if (ret <= 0)
      break;
  }

  /* free elements2clusters array */
  free(elements2clusters);

  return clusters;
}
