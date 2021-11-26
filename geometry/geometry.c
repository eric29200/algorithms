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

  if (!points || nb_points == 0)
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

