#include <string.h>

#include "wkb_reader.h"
#include "../utils/mem.h"
#include "../utils/math.h"

/*
 * Read WKB points.
 */
static int wkb_read_points(void *wkb, size_t *wkb_len, struct point_t *points, size_t nb_points)
{
  size_t i;

  /* read points */
  memcpy(points, wkb + *wkb_len, sizeof(struct point_t) * nb_points);
  *wkb_len += sizeof(struct point_t) * nb_points;

  /* convert IEEE754 to host */
  for (i = 0; i < nb_points; i++) {
    points[i].x = ieee754_binary64_decode((char *) &points[i].x);
    points[i].y = ieee754_binary64_decode((char *) &points[i].y);
  }

  return 0;
}

/*
 * Read WKB ring.
 */
static int wkb_read_ring(void *wkb, size_t *wkb_len, struct ring_t *ring, char endian)
{
  /* read number of points */
  ring->nb_points = WKB_ENDIAN(*((int *) (wkb + *wkb_len)), endian);
  *wkb_len += sizeof(int);

  /* allocate points */
  ring->points = (struct point_t *) xmalloc(sizeof(struct point_t) * ring->nb_points);

  /* read points */
  return wkb_read_points(wkb, wkb_len, ring->points, ring->nb_points);
}

/*
 * Read WKB polygon.
 */
static int wkb_read_polygon(void *wkb, size_t *wkb_len, struct polygon_t *polygon, char endian)
{
  size_t i;
  int ret;

  /* read number of rings */
  polygon->nb_rings = WKB_ENDIAN(*((int *) (wkb + *wkb_len)), endian);
  *wkb_len += sizeof(int);

  /* allocate rings */
  polygon->rings = (struct ring_t *) xmalloc(sizeof(struct ring_t) * polygon->nb_rings);

  /* read rings */
  for (i = 0; i < polygon->nb_rings; i++) {
    ret = wkb_read_ring(wkb, wkb_len, &polygon->rings[i], endian);
    if (ret != 0)
      return ret;
  }

  return 0;
}

/*
 * Read WKB multi polygon.
 */
static int wkb_read_multipolygon(void *wkb, size_t *wkb_len, struct multi_polygon_t *multi_polygon, char endian)
{
  int geometry_type, ret;
  char polygon_endian;
  size_t i;

  /* read number of polygons */
  multi_polygon->nb_polygons = WKB_ENDIAN(*((int *) (wkb + *wkb_len)), endian);
  *wkb_len += sizeof(int);

  /* allocate polygons */
  multi_polygon->polygons = (struct polygon_t *) xmalloc(sizeof(struct polygon_t) * multi_polygon->nb_polygons);

  /* read polygons */
  for (i = 0; i < multi_polygon->nb_polygons; i++) {
    /* read byte order */
    polygon_endian = *((char *) (wkb + *wkb_len));
    *wkb_len += sizeof(char);

    /* read geometry type */
    geometry_type = WKB_ENDIAN(*((int *) (wkb + *wkb_len)), polygon_endian);
    *wkb_len += sizeof(int);

    /* geometry must be a polygon */
    if (geometry_type != GEOMETRY_POLYGON)
      return ERRWKB;

    /* read polygon */
    ret = wkb_read_polygon(wkb, wkb_len, &multi_polygon->polygons[i], polygon_endian);
    if (ret != 0)
      return ret;
  }

  return 0;
}

/*
 * Read and create WKB geometry.
 */
struct geometry_t *wkb_read_geometry(void *wkb, size_t *wkb_len)
{
  struct geometry_t *geometry;
  char endian;

  /* reset wkb length */
  *wkb_len = 0;

  /* allocate geometry */
  geometry = (struct geometry_t *) xmalloc(sizeof(struct geometry_t));

  /* read byte order */
  endian = *((char *) (wkb + *wkb_len));
  *wkb_len += sizeof(char);

  /* read geometry type */
  geometry->type = WKB_ENDIAN(*((int *) (wkb + *wkb_len)), endian);
  *wkb_len += sizeof(int);

  /* get type definition */
  switch (geometry->type) {
    case GEOMETRY_POLYGON:
      if (wkb_read_polygon(wkb, wkb_len, &geometry->u.polygon, endian) != 0)
        goto err;
      break;
    case GEOMETRY_MULTIPOLYGON:
      if (wkb_read_multipolygon(wkb, wkb_len, &geometry->u.multi_polygon, endian) != 0)
        goto err;
      break;
    default:
      goto err;
  }

  return geometry;
err:
  geometry_free(geometry);
  return NULL;
}

