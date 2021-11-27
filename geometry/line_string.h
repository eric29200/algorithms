#ifndef _LINE_STRING_H_
#define _LINE_STRING_H_

#include <stdio.h>

#include "geometry.h"

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
  struct geometry_t **line_strings;
  size_t nb_line_strings;
};

struct geometry_t *line_string_create(struct point_t *points, size_t nb_points);
struct geometry_t *multi_line_string_create(struct geometry_t **line_strings, size_t nb_line_strings);
void line_string_free(struct line_string_t *line_string);
void multi_line_string_free(struct multi_line_string_t *multi_line_string);

int line_string_intersects(struct geometry_t *line_string, struct geometry_t *g);
int multi_line_string_intersects(struct geometry_t *multi_line_string, struct geometry_t *g);

#endif
