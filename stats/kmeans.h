#ifndef _KMEANS_H_
#define _KMEANS_H_

#include <stdio.h>

struct cluster_t {
  void *centroid;
  void *items;
  size_t nb_items;
};

void cluster_free(struct cluster_t *cluster);
struct cluster_t **kmeans(void *items, size_t nb_items, size_t item_size, size_t k,
                          double (*distance_func)(const void *, const void *),
                          void (*mean_func)(void *, size_t, void *));

#endif
