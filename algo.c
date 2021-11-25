#include <stdio.h>

#include "geometry/wkb_reader.h"
#include "data_structures/list.h"

#define INPUT_FILE            "/home/eric/data.wkb"

/*
 * Main.
 */
int main()
{
  struct list_t *geometries = NULL;
  struct geometry_t *geometry;
  FILE *fp;

  /* open wkb file */
  fp = fopen(INPUT_FILE, "r");
  if (!fp) {
    perror("fopen");
    return 1;
  }

  for (;;) {
    geometry = wkb_read_geometry(fp);
    if (!geometry)
      break;

    geometries = list_append(geometries, geometry);
  }

  list_free_full(geometries, geometry_free);

  /* close wkb file */
  fclose(fp);

  return 0;
}
