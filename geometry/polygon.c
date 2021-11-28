#include <stdio.h>

#include "geometry.h"
#include "../utils/mem.h"

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
  envelope_create(geometry);

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
  envelope_create(geometry);

  return geometry;
}

/*
 * Free a linear ring.
 */
void ring_free(struct ring_t *ring)
{
  if (!ring)
    return;

  xfree(ring->points);
}

/*
 * Free a polygon.
 */
void polygon_free(struct polygon_t *polygon)
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
void multi_polygon_free(struct multi_polygon_t *multi_polygon)
{
  size_t i;

  if (!multi_polygon)
    return;

  for (i = 0; i < multi_polygon->nb_polygons; i++)
    geometry_free(multi_polygon->polygons[i]);

  xfree(multi_polygon->polygons);
}

/*
 * Check if polygon contains point p.
 */
static int polygon_contains_point(struct polygon_t *polygon, struct point_t *p)
{
  size_t i, j, nb_intersections;
  struct point_t *p0, *p1;
  struct ring_t *ring;
  double slope;
  int c1, c2;

  /* compute number of intersections with vertical ray */
  for (i = 0, nb_intersections = 0; i < polygon->nb_rings; i++) {
    ring = &polygon->rings[i];

    for (j = 1; j < ring->nb_points; j++) {
      p0 = &ring->points[j - 1];
      p1 = &ring->points[j];

      /* ray out of intersection */
      c1 = p->x >= p0->x && p->x < p1->x;
      c2 = p->x >= p1->x && p->x < p0->x;
      if (!c1 && !c2)
        continue;

      /* check slope */
      slope = (p1->y - p0->y) / (p1->x - p0->x);
      if (p->y < (slope * (p->x - p0->x) + p0->y))
        nb_intersections++;
    }
  }

  /* polygon contains point if number of intersections is even */
  return nb_intersections > 0 && nb_intersections % 2 == 1;
}

/*
 * Check if polygon contains g.
 */
int polygon_contains(struct geometry_t *polygon, struct geometry_t *g)
{
  /* check envelope */
  if (!envelope_contains(polygon->envelope, g))
    return 0;

  if (g->type == GEOMETRY_POINT)
    return polygon_contains_point(&polygon->u.polygon, &g->u.point);

  return 0;
}

/*
 * Check if multi polygon contains g.
 */
int multi_polygon_contains(struct geometry_t *multi_polygon, struct geometry_t *g)
{
  struct geometry_t *polygon;
  size_t i;

  /* check envelope */
  if (!envelope_contains(multi_polygon->envelope, g))
    return 0;

  /* check every polygon */
  for (i = 0; i < multi_polygon->u.multi_polygon.nb_polygons; i++) {
    polygon = multi_polygon->u.multi_polygon.polygons[i];

    if (polygon_contains(polygon, g))
      return 1;
  }

  return 0;
}

/*
 * Check if polygon intersects line string ls.
 */
static int polygon_intersects_line_string(struct polygon_t *polygon, struct line_string_t *ls)
{
  struct point_t *p1, *p2;
  struct ring_t *ring;
  size_t i, j, k;

  /* if polygon contains first point, polygon intersects line string */
  if (polygon_contains_point(polygon, &ls->points[0]))
    return 1;

  /* for each ring */
  for (i = 0; i < polygon->nb_rings; i++) {
    ring = &polygon->rings[i];

    /* for each segment */
    for (j = 1; j < ring->nb_points; j++) {
      p1 = &ring->points[j - 1];
      p2 = &ring->points[j];

      /* check each point of ls */
      for (k = 1; k < ls->nb_points; k++)
        if (segment_intersects(p1, p2, &ls->points[k - 1], &ls->points[k]))
          return 1;
    }
  }

  return 0;
}

/*
 * Check if polygon intersects multi line string mls.
 */
static int polygon_intersects_multi_line_string(struct polygon_t *polygon, struct multi_line_string_t *mls)
{
  size_t i;

  for (i = 0; i < mls->nb_line_strings; i++)
    if (polygon_intersects_line_string(polygon, &mls->line_strings[i]->u.line_string))
      return 1;

  return 0;
}

/*
 * Check if polygon intersects g.
 */
int polygon_intersects(struct geometry_t *polygon, struct geometry_t *g)
{
  /* check envelope first */
  if (!envelope_intersects(polygon->envelope, g))
    return 0;

  if (g->type == GEOMETRY_LINE_STRING)
    return polygon_intersects_line_string(&polygon->u.polygon, &g->u.line_string);
  else if (g->type == GEOMETRY_MULTI_LINE_STRING)
    return polygon_intersects_multi_line_string(&polygon->u.polygon, &g->u.multi_line_string);

  return 0;
}

/*
 * Check if multi polygon intersects g.
 */
int multi_polygon_intersects(struct geometry_t *multi_polygon, struct geometry_t *g)
{
  size_t i;

  /* check envelope first */
  if (!envelope_intersects(multi_polygon->envelope, g))
    return 0;

  /* check every polygon */
  for (i = 0; i < multi_polygon->u.multi_polygon.nb_polygons; i++)
    if (polygon_intersects(multi_polygon->u.multi_polygon.polygons[i], g))
      return 1;

  return 0;
}
