#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "huffman.h"

/*
 * Usage.
 */
static void usage(const char *name)
{
  fprintf(stderr, "%s input_file\n", name);
}

/*
 * Read all contant of a file.
 */
static char *read_all_file(const char *file)
{
  char *buf = NULL;
  long file_size;
  FILE *fp;

  /* open file */
  fp = fopen(file, "r");
  if (!fp) {
    perror("fopen");
    return NULL;
  }

  /* seek to end */
  if (fseek(fp, 0, SEEK_END) != 0) {
    perror("fseek");
    goto out;
  }

  /* get file size */
  file_size = ftell(fp);
  if (file_size == -1)
    perror("ftell");

  if (file_size > 0) {
    /* rewind */
    rewind(fp);

    /* allocate buffer */
    buf = (char *) malloc(file_size);
    if (!buf)
      goto out;

    /* read all content */
    if (fread(buf, 1, file_size, fp) != file_size) {
      perror("fread");
      free(buf);
      buf = NULL;
    }
  }

out:
  fclose(fp);
  return buf;
}

int main(int argc, char **argv)
{
  char *buf;

  /* check arguments */
  if (argc != 2) {
    usage(argv[0]);
    return 1;
  }

  /* read input file */
  buf = read_all_file(argv[1]);
  if (!buf)
    return 1;

  /* huffman encode */
  huffman_encode(buf);

  /* free buffer */
  free(buf);

  return 0;
}
