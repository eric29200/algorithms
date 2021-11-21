/*
 * LZ77 algorithm = lossless data compression algorithm.
 * This algorithm maintains a sliding window (user defined parameter).
 * 1 - read first window of input file and write it exactly to the output file
 * 2 - try to find a matching pattern of next characher in the window
 *     -> if it matches, write window reference (offset), pattern length and next character
 *     -> else write 0,0 and current character
 */
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
  int ret, i, match, shift, len, buf_size;
  FILE *fp_input, *fp_output;

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
    for (i = buf_size - WINDOW_SIZE - 1, match = -1; i > 0; i--) {
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
      fputc(look_ahead[0], fp_output);
      shift = 1;
    } else {
      fputc(WINDOW_SIZE - match, fp_output);
      fputc(i, fp_output);
      fputc(look_ahead[i], fp_output);
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

/*
 * Uncompress a file with lz77 algorithm.
 */
int lz77_uncompress(const char *input_file, const char *output_file)
{
  unsigned char window[WINDOW_SIZE], buf_in[3], buf_out[WINDOW_SIZE];
  int ret, offset, i, j, shift, len;
  FILE *fp_input, *fp_output;

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

  /* write first uncompressed window */
  len = fread(window, 1, WINDOW_SIZE, fp_input);
  fwrite(window, 1, len, fp_output);

  /* end of file */
  if (len != WINDOW_SIZE)
    goto out;

  /* lz77 algorithm */
  while (1) {
    /* read next 2 characters */
    len = fread(buf_in, 1, 3, fp_input);
    if (len != 3)
      break;

    /* extract pattern offset, length and next char */
    offset = (int) buf_in[0];
    len = (int) buf_in[1];
    shift = len + 1;

    /* decode and write pattern */
    if (len > 0) {
      /* decode pattern */
      for (i = 0, j = WINDOW_SIZE - offset; i < len; i++, j++)
        buf_out[i] = window[j];

      /* write pattern */
      fwrite(buf_out, 1, len, fp_output);
    }

    /* write next character */
    fputc(buf_in[2], fp_output);

    /* shift window */
    for (i = 0; i < WINDOW_SIZE - shift; i++)
      window[i] = window[i + shift];

    /* copy next characters from output buffer to window */
    for (i = 0, j = WINDOW_SIZE - shift; i < len; i++, j++)
      window[j] = buf_out[i];
    window[WINDOW_SIZE - 1] = buf_in[2];
  }

out:
  /* close files */
  fclose(fp_input);
  fclose(fp_output);

  return 0;
}
