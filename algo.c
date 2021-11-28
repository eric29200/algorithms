#include <stdio.h>
#include <stdlib.h>

#include "geometry/wkb_reader.h"
#include "data_structures/list.h"
#include "utils/mem.h"

#define INPUT_FILE            "/home/eric/data.wkb"
#define NB_TESTS              100000

/*
 * Read geometries.
 */
struct geometry_t **geometries_read(size_t *nb_geometries)
{
  struct list_t *geometries_list = NULL, *it;
  struct geometry_t **geometries = NULL;
  size_t file_size, wkb_len, len, i;
  struct geometry_t *geometry;
  char *buf;
  FILE *fp;

  /* reset number of geometries */
  *nb_geometries = 0;

  /* open input file */
  fp = fopen(INPUT_FILE, "rb");
  if (!fp) {
    perror("fopen");
    return NULL;
  }

  /* get file length */
  fseek(fp, 0, SEEK_END);
  file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  /* allocate buffer */
  buf = (char *) xmalloc(file_size + 1);

  /* read all file in memory */
  if (fread(buf, sizeof(char), file_size, fp) != file_size) {
    perror("fread");
    goto out;
  }

  /* end buffer */
  buf[file_size] = 0;

  /* read geometries */
  for (len = 0, geometries_list = NULL; len < file_size; len += wkb_len, *nb_geometries += 1) {
    /* read geometry */
    geometry = wkb_read_geometry(buf + len, &wkb_len);
    if (!geometry)
      goto out;

    /* add geometry */
    geometries_list = list_append(geometries_list, geometry);
  }

  /* convert list to array */
  geometries = (struct geometry_t **) xmalloc(sizeof(struct geometry_t *) * *nb_geometries);
  for (i = 0, it = geometries_list; i < *nb_geometries; i++, it = it->next)
    geometries[i] = (struct geometry_t *) it->data;

out:
  /* free geometries */
  list_free(geometries_list);

  /* free buffer */
  free(buf);

  /* close input file */
  fclose(fp);

  return geometries;
}

/*
 * Main.
 */
int main()
{
  struct geometry_t **geometries, *line_string;
  struct point_t *points;
  size_t nb_geometries, i, j;

  /* read geometries */
  geometries = geometries_read(&nb_geometries);
  if (!geometries)
    return -1;

  /* create line string */
  points = (struct point_t *) xmalloc(sizeof(struct point_t) * 2);
  points[0].x = -5.004;
  points[0].y = 48.198;
  points[1].x = -6.004;
  points[1].y = 49.198;
  line_string = line_string_create(points, 2);

  /* get geometries intersecting line string */
  for (i = 0; i < NB_TESTS; i++)
    for (j = 0; j < nb_geometries; j++)
      geometry_intersects(geometries[j], line_string);

  /* free line string */
  geometry_free(line_string);

  /* free geometries */
  if (geometries) {
    for (i = 0; i < nb_geometries; i++)
      geometry_free(geometries[i]);

    free(geometries);
  }

  return 0;
}
