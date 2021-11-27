#include "geometry.h"

/*
 * Check if polygon contains point p.
 */
static int polygon_contains(struct geometry_t *polygon, struct point_t *p)
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
