#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "lz78.h"

/*
 * Usage.
 */
static void usage(const char *name)
{
  fprintf(stderr, "%s input_file output_file new_file\n", name);
}

int main(int argc, char **argv)
{
  /* check arguments */
  if (argc != 4) {
    usage(argv[0]);
    return 1;
  }

  /* lz78 compression */
  lz78_compress(argv[1], argv[2]);

  /* lz78 uncompression */
  //lz78_uncompress(argv[2], argv[3]);

  return 0;
}
