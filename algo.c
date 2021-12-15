#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "compression/huffman.h"
#include "compression/lz77.h"
#include "compression/lz78.h"

/*
 * Compression test.
 */
static void compression_test(const char *input_file, const char *compressed_file, const char *uncompressed_file,
                             const char *method, int (*compression)(const char *, const char *),
                             int (*uncompression)(const char *, const char *))
{
  off_t input_size, output_size;
  struct stat statbuf;
  clock_t start, end;
  double t1, t2;
  int err;

  /* get initial size */
  err = stat(input_file, &statbuf);
  if (err != 0)
    return;
  input_size = statbuf.st_size;

  /* compression */
  start = clock();
  compression(input_file, compressed_file);
  end = clock();
  t1 = (double) (end - start) / CLOCKS_PER_SEC;

  /* uncompression */
  start = clock();
  uncompression(compressed_file, uncompressed_file);
  end = clock();
  t2 = (double) (end - start) / CLOCKS_PER_SEC;

  /* get output size */
  err = stat(compressed_file, &statbuf);
  if (err != 0)
    return;
  output_size = statbuf.st_size;

  /* print statistics */
  printf("******* %s\n", method);
  printf("Ratio : %f\n", (double) input_size / (double) output_size);
  printf("Compression time : %f\n", t1);
  printf("Uncompression time : %f\n", t2);
}

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

  /* huffman */
  compression_test(argv[1], argv[2], argv[3], "Huffman", huffman_encode, huffman_decode);

  /* lz77 */
  compression_test(argv[1], argv[2], argv[3], "LZ77", lz77_compress, lz77_uncompress);

  /* lz78 */
  compression_test(argv[1], argv[2], argv[3], "LZ78", lz78_compress, lz78_uncompress);

  return 0;
}
