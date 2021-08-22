#include <stdio.h>
#include <errno.h>

#include "lz77.h"

#define WINDOW_SIZE       100
#define LOOK_AHEAD_SIZE   120
#define BUFFER_SIZE       ((WINDOW_SIZE) + (LOOK_AHEAD_SIZE))

/*
 * Find a "window" match in string "s" of "len". If a match is found, return position in window.
 * Else return -1.
 */
static inline int lz77_match(unsigned char *window, unsigned char *s, int len)
{
  register int i, j, k;

  for (i = 0; i < WINDOW_SIZE - len; i++) {
    for (j = 0, k = i; j < len; j++)
      if (s[j] == window[k])
        k++;
      else
        break;

    if (j == len)
      return i;
  }

  return -1;
}

/*
 * Compress a file with lz77 algorithm.
 */
int lz77_compress(const char *input_file, const char *output_file)
{
  unsigned char buf[BUFFER_SIZE], *window, *look_ahead;
  FILE *fp_input, *fp_output;
  int ret, i, match, shift;
  size_t len, buf_size;

  /* open input file */
  fp_input = fopen(input_file, "r");
  if (!fp_input)
    return errno;

  /* read first buffer */
  len = fread(buf, 1, BUFFER_SIZE, fp_input);
  if (len < BUFFER_SIZE) {
    fclose(fp_input);
    return -1;
  }

  /* open output file */
  fp_output = fopen(output_file, "w");
  if (!fp_output) {
    ret = errno;
    fclose(fp_input);
    return ret;
  }

  /* write first window directly */
  fwrite(buf, 1, WINDOW_SIZE, fp_output);

  /* lz77 algorithm */
  for (buf_size = BUFFER_SIZE; buf_size > WINDOW_SIZE;) {
    /* set window and look ahead buffer */
    window = buf;
    look_ahead = window + WINDOW_SIZE;

    /* find best match */
    for (i = buf_size, match = -1; i > 0; i--) {
      match = lz77_match(window, look_ahead, i);
      if (match != -1)
        break;
    }

    /*
     * Write result to output file :
     * 1 - relative position of pattern in window
     * 2 - pattern length
     * 3 - next character
     */
    if (match == -1) {
      fputc(0, fp_output);
      fputc(0, fp_output);
      fputc(buf[0], fp_output);
      shift = 1;
    } else {
      fputc(WINDOW_SIZE - match, fp_output);
      fputc(i, fp_output);
      fputc(buf[i], fp_output);
      shift = i + 1;
    }

    /* shift buffer */
    for (i = 0; i < buf_size - shift; i++)
      buf[i] = buf[i + shift];

    /* read next bytes */
    if (buf_size == BUFFER_SIZE)
      buf_size = BUFFER_SIZE - shift + fread(&buf[BUFFER_SIZE - shift], 1, shift, fp_input);
    else
      buf_size -= shift;
  }

  /* close files */
  fclose(fp_input);
  fclose(fp_output);

  return 0;
}
