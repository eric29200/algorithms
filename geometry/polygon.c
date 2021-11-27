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
int polygon_contains(struct geometry_t *polygon, struct point_t *p)
{
  size_t i, j, nb_intersections;
  struct point_t *p0, *p1;
  struct ring_t *ring;
  double slope;
  int c1, c2;

  if (!polygon || !p)
    return 0;

  /* check envelope first */
  if (p->x < polygon->envelope->points[0].x || p->x > polygon->envelope->points[2].x
   || p->y < polygon->envelope->points[0].y || p->y > polygon->envelope->points[2].y)
    return 0;

  /* compute number of intersections with vertical ray */
  for (i = 0, nb_intersections = 0; i < polygon->u.polygon.nb_rings; i++) {
    ring = &polygon->u.polygon.rings[i];

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
 * Check if multi polygon contains point p.
 */
int multi_polygon_contains(struct geometry_t *multi_polygon, struct point_t *p)
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
