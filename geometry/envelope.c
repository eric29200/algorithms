#include <math.h>

#include "envelope.h"
#include "../utils/mem.h"

/*
 * Compute geometry envelope.
 */
void envelope_create(struct geometry_t *geometry)
{
  struct point_t min_point = { INFINITY, INFINITY }, max_point = { -INFINITY, -INFINITY };
  struct line_string_t *line_string;
  struct polygon_t *polygon;
  size_t i;

  /* envelope already computed */
  if (!geometry)
    return;

  switch (geometry->type) {
    case GEOMETRY_POINT:
      return;
    case GEOMETRY_LINE_STRING:
      points_min_max(geometry->u.line_string.points, geometry->u.line_string.nb_points, &min_point, &max_point);
      break;
    case GEOMETRY_POLYGON:
      points_min_max(geometry->u.polygon.rings[0].points, geometry->u.polygon.rings[0].nb_points, &min_point, &max_point);
      break;
    case GEOMETRY_MULTI_POINT:
      for (i = 0; i < geometry->u.multi_point.nb_points; i++)
        points_min_max(&geometry->u.multi_point.points[i]->u.point, 1, &min_point, &max_point);
      break;
    case GEOMETRY_MULTI_LINE_STRING:
      for (i = 0; i < geometry->u.multi_line_string.nb_line_strings; i++) {
        /* compute line string envelope */
        envelope_create(geometry->u.multi_line_string.line_strings[i]);

        /* get line string */
        line_string = &geometry->u.multi_line_string.line_strings[i]->u.line_string;

        /* update min/max points */
        points_min_max(line_string->points, line_string->nb_points, &min_point, &max_point);
      }

      break;
    case GEOMETRY_MULTI_POLYGON:
      for (i = 0; i < geometry->u.multi_polygon.nb_polygons; i++) {
        /* compute polygon envelope */
        envelope_create(geometry->u.multi_polygon.polygons[i]);

        /* get polygon */
        polygon = &geometry->u.multi_polygon.polygons[i]->u.polygon;

        /* update min/max points */
        points_min_max(polygon->rings[0].points, polygon->rings[0].nb_points, &min_point, &max_point);
      }

      break;
  }

  /* create envelope */
  geometry->envelope = (struct envelope_t *) xmalloc(sizeof(struct envelope_t));
  geometry->envelope->x_min = min_point.x;
  geometry->envelope->x_max = max_point.x;
  geometry->envelope->y_min = min_point.y;
  geometry->envelope->y_max = max_point.y;
}

/*
 * Check if envelope contains point p.
 */
int envelope_contains_point(struct envelope_t *envelope, struct point_t *p)
{
  return p->x >= envelope->x_min && p->y >= envelope->y_min
      && p->x <= envelope->x_max && p->y <= envelope->y_max;
}

/*
 * Check if envelope contains line string ls.
 */
int envelope_contains_line_string(struct envelope_t *envelope, struct line_string_t *ls)
{
  size_t i;

  for (i = 0; i < ls->nb_points; i++)
    if (!envelope_contains_point(envelope, &ls->points[i]))
      return 0;

  return 1;
}

/*
 * Check if envelope contains multi line string mls.
 */
int envelope_contains_multi_line_string(struct envelope_t *envelope, struct multi_line_string_t *mls)
{
  size_t i;

  for (i = 0; i < mls->nb_line_strings; i++)
    if (!envelope_contains_line_string(envelope, &mls->line_strings[i]->u.line_string))
      return 0;

  return 1;
}

/*
 * Check if envelope contains g.
 */
int envelope_contains(struct envelope_t *envelope, struct geometry_t *g)
{
  if (g->type == GEOMETRY_POINT)
    return envelope_contains_point(envelope, &g->u.point);

  return 0;
}
