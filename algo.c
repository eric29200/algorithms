#include <stdio.h>
#include <stdlib.h>

#include "geometry/wkb_reader.h"
#include "data_structures/list.h"
#include "utils/mem.h"

#define INPUT_FILE            "/home/eric/data.wkb"

/*
 * Main.
 */
int main()
{
  struct list_t *geometries = NULL;
  struct geometry_t *geometry;
  size_t file_size, wkb_len;
  char *buf, *buf_ptr;
  FILE *fp;

  /* open input file */
  fp = fopen(INPUT_FILE, "rb");
  if (!fp) {
    perror("fopen");
    return -1;
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
  for (buf_ptr = buf, geometries = NULL; ; buf_ptr += wkb_len) {
    /* read geometry */
    geometry = wkb_read_geometry(buf_ptr, &wkb_len);
    if (!geometry)
      break;

    /* add geometry */
    geometries = list_append(geometries, geometry);
  }

out:
  /* free geometries */
  list_free_full(geometries, (void (*)) geometry_free);

  /* free buffer */
  free(buf);

  /* close input file */
  fclose(fp);

  return 0;
}
