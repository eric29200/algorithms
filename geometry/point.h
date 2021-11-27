#ifndef _POINT_H_
#define _POINT_H_

#include <stdio.h>

#include "geometry.h"

/*
 * Point structure.
 */
struct point_t {
  double x;
  double y;
};

/*
 * Multi points structure.
 */
struct multi_point_t {
  struct geometry_t **points;
  size_t nb_points;
};

struct geometry_t *point_create(double x, double y);
struct geometry_t *multi_point_create(struct geometry_t **points, size_t nb_points);
void multi_point_free(struct multi_point_t *multi_point);

void points_min_max(struct point_t *points, size_t nb_points, struct point_t *point_min, struct point_t *point_max);
int points_orientation(struct point_t *p1, struct point_t *p2, struct point_t *p3);
int segment_intersects(struct point_t *p1, struct point_t *p2, struct point_t *p3, struct point_t *p4);

#endif
