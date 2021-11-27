#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <stdio.h>

#include "point.h"
#include "line_string.h"
#include "polygon.h"

#define GEOMETRY_POINT              1
#define GEOMETRY_LINE_STRING        2
#define GEOMETRY_POLYGON            3
#define GEOMETRY_MULTI_POINT        4
#define GEOMETRY_MULTI_LINE_STRING  5
#define GEOMETRY_MULTI_POLYGON      6

/*
 * Geometry structure.
 */
struct geometry_t {
  int type;
  union {
    struct point_t point;
    struct line_string_t line_string;
    struct polygon_t polygon;
    struct multi_point_t multi_point;
    struct multi_line_string_t multi_line_string;
    struct multi_polygon_t multi_polygon;
  } u;
  struct ring_t *envelope;
};

struct geometry_t *geometry_create(int type);
void geometry_free(struct geometry_t *geometry);
void geometry_compute_envelope(struct geometry_t *geometry);
int geometry_contains(struct geometry_t *g1, struct point_t *p);

#endif
