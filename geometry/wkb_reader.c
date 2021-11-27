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
 * Read WKB line string.
 */
static int wkb_read_line_string(void *wkb, size_t *wkb_len, struct line_string_t *line_string, char endian)
{
  /* read number of points */
  line_string->nb_points = WKB_ENDIAN(*((int *) (wkb + *wkb_len)), endian);
  *wkb_len += sizeof(int);

  /* allocate points */
  line_string->points = (struct point_t *) xmalloc(sizeof(struct point_t) * line_string->nb_points);

  /* read points */
  return wkb_read_points(wkb, wkb_len, line_string->points, line_string->nb_points);
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
 * Read WKB multi point.
 */
static int wkb_read_multi_point(void *wkb, size_t *wkb_len, struct multi_point_t *multi_point, char endian)
{
  size_t i, len;

  /* read number of points */
  multi_point->nb_points = WKB_ENDIAN(*((int *) (wkb + *wkb_len)), endian);
  *wkb_len += sizeof(int);

  /* allocate points */
  multi_point->points = (struct geometry_t **) xmalloc(sizeof(struct geometry_t *) * multi_point->nb_points);

  /* read points */
  for (i = 0; i < multi_point->nb_points; i++) {
    /* read polygon */
    multi_point->points[i] = wkb_read_geometry(wkb + *wkb_len, &len);
    if (!multi_point->points[i])
      return ERRWKB;

    /* update wkb len */
    *wkb_len += len;
  }

  return 0;
}

/*
 * Read WKB multi line string.
 */
static int wkb_read_multi_line_string(void *wkb, size_t *wkb_len, struct multi_line_string_t *multi_line_string,
                                      char endian)
{
  size_t i, len;

  /* read number of line strings */
  multi_line_string->nb_line_strings = WKB_ENDIAN(*((int *) (wkb + *wkb_len)), endian);
  *wkb_len += sizeof(int);

  /* allocate line strings */
  multi_line_string->line_strings = (struct geometry_t **) xmalloc(sizeof(struct geometry_t *)
                                                                   * multi_line_string->nb_line_strings);

  /* read line strings */
  for (i = 0; i < multi_line_string->nb_line_strings; i++) {
    /* read polygon */
    multi_line_string->line_strings[i] = wkb_read_geometry(wkb + *wkb_len, &len);
    if (!multi_line_string->line_strings[i])
      return ERRWKB;

    /* update wkb len */
    *wkb_len += len;
  }

  return 0;
}

/*
 * Read WKB multi polygon.
 */
static int wkb_read_multi_polygon(void *wkb, size_t *wkb_len, struct multi_polygon_t *multi_polygon, char endian)
{
  size_t i, len;

  /* read number of polygons */
  multi_polygon->nb_polygons = WKB_ENDIAN(*((int *) (wkb + *wkb_len)), endian);
  *wkb_len += sizeof(int);

  /* allocate polygons */
  multi_polygon->polygons = (struct geometry_t **) xmalloc(sizeof(struct geometry_t *) * multi_polygon->nb_polygons);

  /* read polygons */
  for (i = 0; i < multi_polygon->nb_polygons; i++) {
    /* read polygon */
    multi_polygon->polygons[i] = wkb_read_geometry(wkb + *wkb_len, &len);
    if (!multi_polygon->polygons[i])
      return ERRWKB;

    /* update wkb len */
    *wkb_len += len;
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
    case GEOMETRY_POINT:
      if (wkb_read_points(wkb, wkb_len, &geometry->u.point, 1) != 0)
        goto err;
      break;
    case GEOMETRY_LINE_STRING:
      if (wkb_read_line_string(wkb, wkb_len, &geometry->u.line_string, endian) != 0)
        goto err;
      break;
    case GEOMETRY_POLYGON:
      if (wkb_read_polygon(wkb, wkb_len, &geometry->u.polygon, endian) != 0)
        goto err;
      break;
    case GEOMETRY_MULTI_POINT:
      if (wkb_read_multi_point(wkb, wkb_len, &geometry->u.multi_point, endian) != 0)
        goto err;
      break;
    case GEOMETRY_MULTI_LINE_STRING:
      if (wkb_read_multi_line_string(wkb, wkb_len, &geometry->u.multi_line_string, endian) != 0)
        goto err;
      break;
    case GEOMETRY_MULTI_POLYGON:
      if (wkb_read_multi_polygon(wkb, wkb_len, &geometry->u.multi_polygon, endian) != 0)
        goto err;
      break;
    default:
      goto err;
  }

  /* compute envelope */
  envelope_create(geometry);

  return geometry;
err:
  geometry_free(geometry);
  return NULL;
}

