#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <stdio.h>

#define GEOMETRY_POINT              1
#define GEOMETRY_LINE_STRING        2
#define GEOMETRY_POLYGON            3
#define GEOMETRY_MULTI_POINT        4
#define GEOMETRY_MULTI_LINE_STRING  5
#define GEOMETRY_MULTI_POLYGON      6

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
  struct point_t *points;
  size_t nb_points;
};

/*
 * Line string structure.
 */
struct line_string_t {
  struct point_t *points;
  size_t nb_points;
};

/*
 * Multi line strings structure.
 */
struct multi_line_string_t {
  struct line_string_t *line_strings;
  size_t nb_line_strings;
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
    struct line_string_t line_string;
    struct polygon_t polygon;
    struct multi_point_t multi_point;
    struct multi_line_string_t multi_line_string;
    struct multi_polygon_t multi_polygon;
  } u;
};

void geometry_free(struct geometry_t *geometry);

#endif
