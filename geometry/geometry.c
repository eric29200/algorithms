#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "geometry.h"
#include "../utils/mem.h"

/*
 * Create a geometry.
 */
struct geometry_t *geometry_create(int type)
{
  struct geometry_t *geometry;

  geometry = (struct geometry_t *) xmalloc(sizeof(struct geometry_t));
  geometry->type = type;
  geometry->envelope = NULL;

  return geometry;
}

/*
 * Compute geometry envelope.
 */
void geometry_compute_envelope(struct geometry_t *geometry)
{
  struct point_t min_point = { INFINITY, INFINITY }, max_point = { -INFINITY, -INFINITY };
  struct line_string_t *line_string;
  struct point_t *envelope_points;
  struct polygon_t *polygon;
  size_t i;

  /* envelope already computed */
  if (!geometry || geometry->envelope)
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
        geometry_compute_envelope(geometry->u.multi_line_string.line_strings[i]);

        /* get line string */
        line_string = &geometry->u.multi_line_string.line_strings[i]->u.line_string;

        /* update min/max points */
        points_min_max(line_string->points, line_string->nb_points, &min_point, &max_point);
      }

      break;
    case GEOMETRY_MULTI_POLYGON:
      for (i = 0; i < geometry->u.multi_polygon.nb_polygons; i++) {
        /* compute polygon envelope */
        geometry_compute_envelope(geometry->u.multi_polygon.polygons[i]);

        /* get polygon */
        polygon = &geometry->u.multi_polygon.polygons[i]->u.polygon;

        /* update min/max points */
        points_min_max(polygon->rings[0].points, polygon->rings[0].nb_points, &min_point, &max_point);
      }

      break;
  }

  /* set envelope */
  envelope_points = (struct point_t *) xmalloc(sizeof(struct point_t) * 5);
  envelope_points[0].x = min_point.x;
  envelope_points[0].y = min_point.y;
  envelope_points[1].x = min_point.x;
  envelope_points[1].y = max_point.y;
  envelope_points[2].x = max_point.x;
  envelope_points[2].y = max_point.y;
  envelope_points[3].x = max_point.x;
  envelope_points[3].y = min_point.y;
  envelope_points[4].x = min_point.x;
  envelope_points[4].y = min_point.y;

  /* create envelope */
  geometry->envelope = ring_create(envelope_points, 5);
}

/*
 * Free a polygon.
 */
void geometry_free(struct geometry_t *geometry)
{
  if (!geometry)
    return;

  switch (geometry->type) {
    case GEOMETRY_POINT:
      break;
    case GEOMETRY_LINE_STRING:
      line_string_free(&geometry->u.line_string);
      break;
    case GEOMETRY_POLYGON:
      polygon_free(&geometry->u.polygon);
      break;
    case GEOMETRY_MULTI_POINT:
      multi_point_free(&geometry->u.multi_point);
      break;
    case GEOMETRY_MULTI_LINE_STRING:
      multi_line_string_free(&geometry->u.multi_line_string);
      break;
    case GEOMETRY_MULTI_POLYGON:
      multi_polygon_free(&geometry->u.multi_polygon);
      break;
    default:
      break;
  }

  /* free envelope */
  if (geometry->envelope) {
    ring_free(geometry->envelope);
    free(geometry->envelope);
  }

  xfree(geometry);
}

/*
 * Check if g1 contains point p.
 */
int geometry_contains(struct geometry_t *g1, struct point_t *p)
{
  int ret;

  if (!g1 || !p)
    return 0;

  switch (g1->type) {
    case GEOMETRY_POLYGON:
      ret = polygon_contains(g1, p);
      break;
    case GEOMETRY_MULTI_POLYGON:
      ret = multi_polygon_contains(g1, p);
      break;
    default:
      ret = 0;
      break;
  }

  return ret;
}
