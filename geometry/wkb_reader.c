#include "wkb_reader.h"
#include "../utils/mem.h"
#include "../utils/math.h"

/*
 * Read WKB points.
 */
static int wkb_read_points(FILE *fp, struct point_t *points, size_t nb_points)
{
  size_t i;

  /* read points */
  if (fread(points, sizeof(struct point_t), nb_points, fp) != nb_points)
    return ERRWKB;

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
static int wkb_read_ring(FILE *fp, struct ring_t *ring, char endian)
{
  /* read number of points */
  if (fread(&ring->nb_points, sizeof(int), 1, fp) != 1)
    return ERRWKB;

  /* convert endianess */
  ring->nb_points = WKB_ENDIAN(ring->nb_points, endian);

  /* allocate points */
  ring->points = (struct point_t *) xmalloc(sizeof(struct point_t) * ring->nb_points);

  /* read points */
  return wkb_read_points(fp, ring->points, ring->nb_points);
}

/*
 * Read WKB polygon.
 */
static int wkb_read_polygon(FILE *fp, struct polygon_t *polygon, char endian)
{
  size_t i;
  int ret;

  /* read number of rings */
  if (fread(&polygon->nb_rings, sizeof(int), 1, fp) != 1)
    return ERRWKB;

  /* convert endianess */
  polygon->nb_rings = WKB_ENDIAN(polygon->nb_rings, endian);

  /* allocate rings */
  polygon->rings = (struct ring_t *) xmalloc(sizeof(struct ring_t) * polygon->nb_rings);

  /* read rings */
  for (i = 0; i < polygon->nb_rings; i++) {
    ret = wkb_read_ring(fp, &polygon->rings[i], endian);
    if (ret != 0)
      return ret;
  }

  return 0;
}

/*
 * Read WKB multi polygon.
 */
static int wkb_read_multipolygon(FILE *fp, struct multi_polygon_t *multi_polygon, char endian)
{
  int ret, geometry_type;
  char polygon_endian;
  size_t i;

  /* read number of polygons */
  if (fread(&multi_polygon->nb_polygons, sizeof(int), 1, fp) != 1)
    return ERRWKB;

  /* convert endianess */
  multi_polygon->nb_polygons = WKB_ENDIAN(multi_polygon->nb_polygons, endian);

  /* allocate polygons */
  multi_polygon->polygons = (struct polygon_t *) xmalloc(sizeof(struct polygon_t) * multi_polygon->nb_polygons);

  /* read polygons */
  for (i = 0; i < multi_polygon->nb_polygons; i++) {
    /* read byte order */
    if (fread(&polygon_endian, sizeof(char), 1, fp) != 1)
      return ERRWKB;

    /* read geometry type */
    if (fread(&geometry_type, sizeof(int), 1, fp) != 1)
      return ERRWKB;

    /* convert endianess */
    geometry_type = WKB_ENDIAN(geometry_type, polygon_endian);

    /* geometry must be a polygon */
    if (geometry_type != GEOMETRY_POLYGON)
      return ERRWKB;

    /* read polygon */
    ret = wkb_read_polygon(fp, &multi_polygon->polygons[i], polygon_endian);
    if (ret != 0)
      return ret;
  }

  return 0;
}

/*
 * Read and create WKB geometry.
 */
struct geometry_t *wkb_read_geometry(FILE *fp)
{
  struct geometry_t *geometry;
  char endian;

  /* allocate geometry */
  geometry = (struct geometry_t *) xmalloc(sizeof(struct geometry_t));

  /* read byte order */
  if (fread(&endian, sizeof(char), 1, fp) != 1)
    goto err;

  /* read geometry type */
  if (fread(&geometry->type, sizeof(int), 1, fp) != 1)
    goto err;

  /* convert endianess */
  geometry->type = WKB_ENDIAN(geometry->type, endian);

  /* get type definition */
  switch (geometry->type) {
    case GEOMETRY_POLYGON:
      if (wkb_read_polygon(fp, &geometry->u.polygon, endian) != 0)
        goto err;
      break;
    case GEOMETRY_MULTIPOLYGON:
      if (wkb_read_multipolygon(fp, &geometry->u.multi_polygon, endian) != 0)
        goto err;
      break;
    default:
      goto err;
  }

  return geometry;
err:
  return NULL;
}

