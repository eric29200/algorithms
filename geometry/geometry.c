#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "geometry.h"
#include "../utils/mem.h"

/*
 * Create a geometry.
 */
static struct geometry_t *geometry_create(int type)
{
  struct geometry_t *geometry;

  geometry = (struct geometry_t *) xmalloc(sizeof(struct geometry_t));
  geometry->type = type;
  geometry->envelope = NULL;

  return geometry;
}

/*
 * Get min/max x/y.
 */
static void points_min_max(struct point_t *points, size_t nb_points, struct point_t *point_min, struct point_t *point_max)
{
  size_t i;

  for (i = 0; i < nb_points; i++) {
    if (points[i].x < point_min->x)
      point_min->x = points[i].x;

    if (points[i].x > point_max->x)
      point_max->x = points[i].x;

    if (points[i].y < point_min->y)
      point_min->y = points[i].y;

    if (points[i].y > point_max->y)
      point_max->y = points[i].y;
  }
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
 * Create a point.
 */
struct geometry_t *point_create(double x, double y)
{
  struct geometry_t *geometry;

  geometry = geometry_create(GEOMETRY_POINT);
  geometry->u.point.x = x;
  geometry->u.point.y = y;

  return geometry;
}

/*
 * Create a line string.
 */
struct geometry_t *line_string_create(struct point_t *points, size_t nb_points)
{
  struct geometry_t *geometry;

  if (!points || nb_points <= 1)
    return NULL;

  geometry = geometry_create(GEOMETRY_LINE_STRING);
  geometry->u.line_string.points = points;
  geometry->u.line_string.nb_points = nb_points;

  /* compute envelope */
  geometry_compute_envelope(geometry);

  return geometry;
}

/*
 * Create a polygon.
 */
struct geometry_t *polygon_create(struct ring_t *rings, size_t nb_rings)
{
  struct geometry_t *geometry;

  if (!rings || nb_rings == 0)
    return NULL;

  geometry = geometry_create(GEOMETRY_POLYGON);
  geometry->u.polygon.rings = rings;
  geometry->u.polygon.nb_rings = nb_rings;

  /* compute envelope */
  geometry_compute_envelope(geometry);

  return geometry;
}

/*
 * Create a multi point.
 */
struct geometry_t *multi_point_create(struct geometry_t **points, size_t nb_points)
{
  struct geometry_t *geometry;

  if (!points || nb_points == 0)
    return NULL;

  geometry = geometry_create(GEOMETRY_MULTI_POINT);
  geometry->u.multi_point.points = points;
  geometry->u.multi_point.nb_points = nb_points;

  /* compute envelope */
  geometry_compute_envelope(geometry);

  return geometry;
}

/*
 * Create a multi line string
 */
struct geometry_t *multi_line_string_create(struct geometry_t **line_strings, size_t nb_line_strings)
{
  struct geometry_t *geometry;

  if (!line_strings || nb_line_strings == 0)
    return NULL;

  geometry = geometry_create(GEOMETRY_MULTI_LINE_STRING);
  geometry->u.multi_line_string.line_strings = line_strings;
  geometry->u.multi_line_string.nb_line_strings = nb_line_strings;

  /* compute envelope */
  geometry_compute_envelope(geometry);

  return geometry;
}

/*
 * Create a multi polygon.
 */
struct geometry_t *multi_polygon_create(struct geometry_t **polygons, size_t nb_polygons)
{
  struct geometry_t *geometry;

  if (!polygons || nb_polygons == 0)
    return NULL;

  geometry = geometry_create(GEOMETRY_MULTI_POLYGON);
  geometry->u.multi_polygon.polygons = polygons;
  geometry->u.multi_polygon.nb_polygons = nb_polygons;

  /* compute envelope */
  geometry_compute_envelope(geometry);

  return geometry;
}

/*
 * Create a linear ring.
 */
struct ring_t *ring_create(struct point_t *points, size_t nb_points)
{
  struct ring_t *ring;

  if (!points || nb_points <= 1)
    return NULL;

  ring = (struct ring_t *) xmalloc(sizeof(struct ring_t));
  ring->points = points;
  ring->nb_points = nb_points;

  return ring;
}

/*
 * Free a line string.
 */
static void line_string_free(struct line_string_t *line_string)
{
  if (!line_string)
    return;

  xfree(line_string->points);
}

/*
 * Free a linear ring.
 */
static void ring_free(struct ring_t *ring)
{
  if (!ring)
    return;

  xfree(ring->points);
}

/*
 * Free a polygon.
 */
static void polygon_free(struct polygon_t *polygon)
{
  size_t i;

  if (!polygon)
    return;

  for (i = 0; i < polygon->nb_rings; i++)
    ring_free(&polygon->rings[i]);

  xfree(polygon->rings);
}

/*
 * Free a multi point.
 */
static void multi_point_free(struct multi_point_t *multi_point)
{
  size_t i;

  if (!multi_point)
    return;

  for (i = 0; i < multi_point->nb_points; i++)
    geometry_free(multi_point->points[i]);

  xfree(multi_point->points);
}

/*
 * Free a multi line string.
 */
static void multi_line_string_free(struct multi_line_string_t *multi_line_string)
{
  size_t i;

  if (!multi_line_string)
    return;

  for (i = 0; i < multi_line_string->nb_line_strings; i++)
    geometry_free(multi_line_string->line_strings[i]);

  xfree(multi_line_string->line_strings);
}

/*
 * Free a multi polygon.
 */
static void multi_polygon_free(struct multi_polygon_t *multi_polygon)
{
  size_t i;

  if (!multi_polygon)
    return;

  for (i = 0; i < multi_polygon->nb_polygons; i++)
    geometry_free(multi_polygon->polygons[i]);

  xfree(multi_polygon->polygons);
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
 * Check if segments (p1 ; p2) and (p3 ; p4) intersect.
 */
static int segment_intersect(struct point_t *p1, struct point_t *p2, struct point_t *p3, struct point_t *p4)
{
  double denom, ua, ub;

  /* parallel lines */
  denom = (p4->y - p3->y) * (p2->x - p1->x) - (p4->x - p3->x) * (p2->y - p1->y);
  if (denom == 0.0)
    return 0;

  /* out of range */
  ua = ((p4->x - p3->x) * (p1->y - p3->y) - (p4->y - p3->y) * (p1->x - p3->x)) / denom;
  if (ua < 0.0 || ua > 1.0)
    return 0;

  /* out of range */
  ub = ((p2->x - p1->x) * (p1->y - p3->y) - (p2->y - p1->y) * (p1->x - p3->x)) / denom;
  if (ub < 0.0 || ub > 1.0)
    return 0;

  return 1;
}

/*
 * Check if polygon contains point p.
 */
static int polygon_contains(struct geometry_t *polygon, struct point_t *p)
{
  size_t i, j, nb_intersections;
  struct ring_t *ring;
  struct point_t p0;

  if (!polygon || !p)
    return 0;

  /* check envelope first */
  if (p->x < polygon->envelope->points[0].x || p->x > polygon->envelope->points[2].x
   || p->y < polygon->envelope->points[0].y || p->y > polygon->envelope->points[2].y)
    return 0;

  /* create ray from left to point */
  p0.x = polygon->envelope->points[0].x - 1;
  p0.y = p->y;

  /* compute number of intersections with ray */
  for (i = 0, nb_intersections = 0; i < polygon->u.polygon.nb_rings; i++) {
    ring = &polygon->u.polygon.rings[i];

    for (j = 1; j < ring->nb_points; j++)
      nb_intersections += segment_intersect(&p0, p, &ring->points[j - 1], &ring->points[j]);
  }

  /* polygon contains point if number of intersections is even */
  return nb_intersections > 0 && nb_intersections % 2 == 1;
}

/*
 * Check if multi polygon contains point p.
 */
static int multi_polygon_contains(struct geometry_t *multi_polygon, struct point_t *p)
{
  size_t i;

  if (!multi_polygon || !p)
    return 0;

  /* check envelope first */
  if (p->x < multi_polygon->envelope->points[0].x || p->x > multi_polygon->envelope->points[2].x
   || p->y < multi_polygon->envelope->points[0].y || p->y > multi_polygon->envelope->points[2].y)
    return 0;

  /* check every polygon */
  for (i = 0; i < multi_polygon->u.multi_polygon.nb_polygons; i++)
    if (polygon_contains(multi_polygon->u.multi_polygon.polygons[i], p))
      return 1;

  return 0;
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
