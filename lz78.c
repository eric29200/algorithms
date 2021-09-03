#include <stdio.h>
#include <errno.h>

#include "lz78.h"
#include "trie.h"

/*
 * Compress a file with lz78 algorithm.
 */
int lz78_compress(const char *input_file, const char *output_file)
{
  struct trie_t *root = NULL, *node, *next;
  FILE *fp_input, *fp_output;
  int ret, c, id = 0;

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

  /* insert root node */
  root = trie_insert(root, 0, id++);
  if (!root)
    goto out;

  /* create dictionnary */
  for (node = root;;) {
    /* get next character */
    c = fgetc(fp_input);
    if (c == EOF)
      break;

    /* find character in trie or create it */
    next = trie_find(node, c);
    if (!next) {
      trie_insert(node, c, id++);
      node = root;
    } else {
      node = next;
    }
  }

  trie_print(root);

out:
  /* free dictionnary */
  trie_free(root);

  /* close files */
  fclose(fp_input);
  fclose(fp_output);

  return 0;
}
