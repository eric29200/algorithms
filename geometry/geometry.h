#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <stdio.h>

#define GEOMETRY_POLYGON           3
#define GEOMETRY_MULTIPOLYGON      6

/*
 * Point structure.
 */
struct point_t {
  double x;
  double y;
};

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
  struct polygon_t *polygons;
  size_t nb_polygons;
};

/*
 * Geometry structure.
 */
struct geometry_t {
  int type;
  union {
    struct point_t point;
    struct polygon_t polygon;
    struct multi_polygon_t multi_polygon;
  } u;
};

void geometry_free(struct geometry_t *geometry);

#endif
