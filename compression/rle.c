/*
 * Run Length Encoding algorithm = loseless data compression algorithm.
 * This algorithm find repeating character.
 * For example, string "AAAABBBC" is encoded into "4A3B1C".
 */
#include <stdio.h>
#include <errno.h>

#include "rle.h"

/*
 * Compress a file with rle algorithm.
 */
int rle_compress(const char *input_file, const char *output_file)
{
  FILE *fp_input, *fp_output;
  char c, c_prev;
  int ret, len;

  /* open input file */
  fp_input = fopen(input_file, "r");
  if (!fp_input)
    return errno;

  /* open output file */
  fp_output = fopen(output_file, "w");
  if (!fp_output) {
    ret = errno;
    fclose(fp_input);
    return ret;
  }

  /* get first character */
  c_prev = fgetc(fp_input);
  if (c_prev == EOF)
    goto out;

  /* encode file */
  for (len = 1; c_prev != EOF;) {
    /* get next character */
    c = fgetc(fp_input);

    /* continue repetating character */
    if (c == c_prev && len < (1 << 8)) {
      len++;
      goto next;
    }

    /* write repeating character */
    fputc(len, fp_output);
    fputc(c_prev, fp_output);
    len = 1;

next:
    c_prev = c;
  }

out:
  /* close files */
  fclose(fp_input);
  fclose(fp_output);

  return 0;
}

/*
 * Uncompress a file with rle algorithm.
 */
int rle_uncompress(const char *input_file, const char *output_file)
{
  FILE *fp_input, *fp_output;
  char c, len, i;
  int ret;

  /* open input file */
  fp_input = fopen(input_file, "r");
  if (!fp_input)
    return errno;

  /* open output file */
  fp_output = fopen(output_file, "w");
  if (!fp_output) {
    ret = errno;
    fclose(fp_input);
    return ret;
  }

  for (;;) {
    /* read length */
    len = fgetc(fp_input);
    if (len == EOF)
      break;

    /* read character */
    c = fgetc(fp_input);
    if (c == EOF)
      break;

    /* write characters to output */
    for (i = 0; i < len; i++)
      fputc(c, fp_output);
  }

  /* close files */
  fclose(fp_input);
  fclose(fp_output);

  return 0;
}
