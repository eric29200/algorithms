#include "geometry.h"

#include "../utils/mem.h"

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
  envelope_create(geometry);

  return geometry;
}

/*
 * Free a multi point.
 */
void multi_point_free(struct multi_point_t *multi_point)
{
  size_t i;

  if (!multi_point)
    return;

  for (i = 0; i < multi_point->nb_points; i++)
    geometry_free(multi_point->points[i]);

  xfree(multi_point->points);
}

/*
 * Get min/max x/y.
 */
void points_min_max(struct point_t *points, size_t nb_points, struct point_t *point_min, struct point_t *point_max)
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
 * Compute points orientation :
 * - -1 : clockwise
 * -  1 : counter clockwise
 * -  0 : collinear
 */
int points_orientation(struct point_t *p1, struct point_t *p2, struct point_t *p3)
{
  double area = (p2->x - p1->x) * (p3->y - p1->y) - (p2->y - p1->y) * (p3->x - p1->x);

  if (area < 0)
    return -1;

  if (area > 0)
    return 1;

  return 0;
}

/*
 * Check if segment (p1 ; p2) intersects segment (p3 ; p4).
 */
int segment_intersects(struct point_t *p1, struct point_t *p2, struct point_t *p3, struct point_t *p4)
{
  if (points_orientation(p1, p2, p3) * points_orientation(p1, p2, p4) > 0)
    return 0;

  return points_orientation(p3, p4, p1) * points_orientation(p3, p4, p2) <= 0;
}
