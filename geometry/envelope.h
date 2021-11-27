#ifndef _ENVELOPE_H_
#define _ENVELOPE_H_

#include "geometry.h"

struct envelope_t {
  double x_min;
  double x_max;
  double y_min;
  double y_max;
};

void envelope_create(struct geometry_t *geometry);
int envelope_contains_point(struct envelope_t *envelope, struct point_t *p);
int envelope_contains_line_string(struct envelope_t *envelope, struct line_string_t *ls);
int envelope_contains_multi_line_string(struct envelope_t *envelope, struct multi_line_string_t *mls);
int envelope_contains(struct envelope_t *envelope, struct geometry_t *g);

#endif
