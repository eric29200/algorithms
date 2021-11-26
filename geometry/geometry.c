#include <string.h>

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

  return geometry;
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

  return geometry;
}

/*
 * Create a multi point.
 */
struct geometry_t *multi_point_create(struct point_t *points, size_t nb_points)
{
  struct geometry_t *geometry;

  if (!points || nb_points == 0)
    return NULL;

  geometry = geometry_create(GEOMETRY_MULTI_POINT);
  geometry->u.multi_point.points = points;
  geometry->u.multi_point.nb_points = nb_points;

  return geometry;
}

/*
 * Create a multi line string
 */
struct geometry_t *multi_line_string_create(struct line_string_t *line_strings, size_t nb_line_strings)
{
  struct geometry_t *geometry;

  if (!line_strings || nb_line_strings == 0)
    return NULL;

  geometry = geometry_create(GEOMETRY_MULTI_LINE_STRING);
  geometry->u.multi_line_string.line_strings = line_strings;
  geometry->u.multi_line_string.nb_line_strings = nb_line_strings;

  return geometry;
}

/*
 * Create a multi polygon.
 */
struct geometry_t *multi_polygon_create(struct polygon_t *polygons, size_t nb_polygons)
{
  struct geometry_t *geometry;

  if (!polygons || nb_polygons == 0)
    return NULL;

  geometry = geometry_create(GEOMETRY_MULTI_POLYGON);
  geometry->u.multi_polygon.polygons = polygons;
  geometry->u.multi_polygon.nb_polygons = nb_polygons;

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
 * Free a multi point.
 */
static void multi_point_free(struct multi_point_t *multi_point)
{
  if (!multi_point)
    return;

  xfree(multi_point->points);
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
 * Free a multi line string.
 */
static void multi_line_string_free(struct multi_line_string_t *multi_line_string)
{
  size_t i;

  if (!multi_line_string)
    return;

  for (i = 0; i < multi_line_string->nb_line_strings; i++)
    line_string_free(&multi_line_string->line_strings[i]);
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
 * Free a multi polygon.
 */
static void multi_polygon_free(struct multi_polygon_t *multi_polygon)
{
  size_t i;

  if (!multi_polygon)
    return;

  for (i = 0; i < multi_polygon->nb_polygons; i++)
    polygon_free(&multi_polygon->polygons[i]);

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
static int polygon_contains(struct polygon_t *polygon, struct point_t *p)
{
  size_t i, j, nb_intersections;
  struct ring_t *ring;
  struct point_t p0;

  if (!polygon || !p)
    return 0;

  /* create ray from left to point */
  p0.x = -181.0;
  p0.y = p->y;

  /* compute number of intersections with ray */
  for (i = 0, nb_intersections = 0; i < polygon->nb_rings; i++) {
    ring = &polygon->rings[i];

    for (j = 1; j < ring->nb_points; j++)
      nb_intersections += segment_intersect(&p0, p, &ring->points[j - 1], &ring->points[j]);
  }

  /* polygon contains point if number of intersections is even */
  return nb_intersections > 0 && nb_intersections % 2 == 1;
}

/*
 * Check if multi polygon contains point p.
 */
static int multi_polygon_contains(struct multi_polygon_t *multi_polygon, struct point_t *p)
{
  size_t i, j, k, nb_intersections;
  struct polygon_t *polygon;
  struct ring_t *ring;
  struct point_t p0;

  if (!multi_polygon || !p)
    return 0;

  /* create ray from left to point */
  p0.x = -181.0;
  p0.y = p->y;

  /* compute number of intersections with ray */
  for (i = 0, nb_intersections = 0; i < multi_polygon->nb_polygons; i++) {
    polygon = &multi_polygon->polygons[i];

    for (j = 0; j < polygon->nb_rings; j++) {
      ring = &polygon->rings[j];

      for (k = 1; k < ring->nb_points; k++)
        nb_intersections += segment_intersect(&p0, p, &ring->points[k - 1], &ring->points[k]);
    }
  }

  /* multi polygon contains point if number of intersections is even */
  return nb_intersections > 0 && nb_intersections % 2 == 1;
}

/*
 * Check if g1 contains g2.
 */
int geometry_contains(struct geometry_t *g1, struct geometry_t *g2)
{
  if (!g1 || !g2)
    return 0;

  if (g1->type == GEOMETRY_POLYGON && g2->type == GEOMETRY_POINT)
    return polygon_contains(&g1->u.polygon, &g2->u.point);
  else if (g1->type == GEOMETRY_MULTI_POLYGON && g2->type == GEOMETRY_POINT)
    return multi_polygon_contains(&g1->u.multi_polygon, &g2->u.point);

  return 0;
}
