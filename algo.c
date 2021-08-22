#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "lz77.h"

/*
 * Usage.
 */
static void usage(const char *name)
{
  fprintf(stderr, "%s input_file output_file\n", name);
}

int main(int argc, char **argv)
{
  /* check arguments */
  if (argc != 3) {
    usage(argv[0]);
    return 1;
  }

  /* lz77 compression */
  lz77_compress(argv[1], argv[2]);

  return 0;
}
