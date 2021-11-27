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

  /* free envelope and geometry */
  xfree(geometry->envelope);
  free(geometry);
}

/*
 * Check if g1 contains g2.
 */
int geometry_contains(struct geometry_t *g1, struct geometry_t *g2)
{
  if (g1->type == GEOMETRY_POLYGON)
    return envelope_contains(g1->envelope, g2) && polygon_contains(&g1->u.polygon, g2);
  else if (g1->type == GEOMETRY_MULTI_POLYGON)
    return envelope_contains(g1->envelope, g2) && multi_polygon_contains(&g1->u.multi_polygon, g2);

  return 0;
}

/*
 * Check if g1 intersects g2.
 */
int geometry_intersects(struct geometry_t *g1, struct geometry_t *g2)
{
  if (g1->type == GEOMETRY_LINE_STRING)
    return line_string_intersects(&g1->u.line_string, g2);
  else if (g1->type == GEOMETRY_MULTI_LINE_STRING)
    return multi_line_string_intersects(&g1->u.multi_line_string, g2);
  else if (g1->type == GEOMETRY_POLYGON)
    return envelope_contains(g1->envelope, g2) || polygon_intersects(&g1->u.polygon, g2);
  else if (g1->type == GEOMETRY_MULTI_POLYGON)
    return envelope_contains(g1->envelope, g2) || multi_polygon_intersects(&g1->u.multi_polygon, g2);

  return 0;
}
