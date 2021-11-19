#ifndef _KMEANS_H_
#define _KMEANS_H_

#include <stdio.h>

struct cluster_t {
  void *centroid;
  void *elements;
  size_t nb_elements;
};

void cluster_free(struct cluster_t *cluster);
struct cluster_t **kmeans(void *elements, size_t nb_elements, size_t element_size, size_t k,
                          double (*distance_func)(const void *, const void *),
                          void (*mean_func)(void *, size_t, void *));

#endif
