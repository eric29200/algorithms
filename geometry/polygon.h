#ifndef _POLYGON_H_
#define _POLYGON_H_

#include <stdio.h>

#include "geometry.h"

/*
 * Linear ring structure.
 */
struct ring_t {
  struct point_t *points;
  size_t nb_points;
};

/*
 * Polygon structure.
 */
struct polygon_t {
  struct ring_t *rings;
  size_t nb_rings;
};

/*
 * Multi polygon structure.
 */
struct multi_polygon_t {
  struct geometry_t **polygons;
  size_t nb_polygons;
};

struct geometry_t *polygon_create(struct ring_t *rings, size_t nb_rings);
struct geometry_t *multi_polygon_create(struct geometry_t **polygons, size_t nb_polygons);
struct ring_t *ring_create(struct point_t *points, size_t nb_points);
void polygon_free(struct polygon_t *polygon);
void multi_polygon_free(struct multi_polygon_t *multi_polygon);
void ring_free(struct ring_t *ring);

int polygon_contains(struct polygon_t *polygon, struct geometry_t *g);
int multi_polygon_contains(struct multi_polygon_t *multi_polygon, struct geometry_t *g);

#endif
