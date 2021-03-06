/*
 * LZ78 algorithm = lossless data compression algorithm.
 * This algorithm maintains a dictionnary (= a trie).
 * 1 - read each character
 * 2 - if the character is already in the dictionnary (start at root), go to next character and update tree node
 *     if the character is not in the dictionnary, add it to the dictionnary and write the previous node id and then character
 * 3 - write final sequence
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "lz78.h"
#include "../data_structures/trie.h"
#include "../utils/mem.h"

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

  /* write temporary dict size */
  fwrite(&id, sizeof(int), 1, fp_output);

  /* insert root node */
  root = trie_insert(root, 0, id++);
  if (!root) {
    ret = -1;
    goto out;
  }

  /* create dictionnary */
  for (node = root;;) {
    /* get next character */
    c = fgetc(fp_input);

    /* find character in trie */
    next = trie_find(node, c);
    if (next) {
      node = next;
      continue;
    }

    /* insert new character in trie */
    trie_insert(node, c, id++);

    /* write compressed data */
    fwrite(&node->id, sizeof(int), 1, fp_output);
    fwrite(&c, sizeof(char), 1, fp_output);

    /* go back to root */
    node = root;

    /* end of file */
    if (c == EOF)
      break;
  }

  /* write final dict size */
  rewind(fp_output);
  fwrite(&id, sizeof(int), 1, fp_output);

  ret = 0;
out:
  /* free dictionnary */
  trie_free(root);

  /* close files */
  fclose(fp_input);
  fclose(fp_output);

  return ret;
}

/*
 * Uncompress a file with lz78 algorithm.
 */
int lz78_uncompress(const char *input_file, const char *output_file)
{
  struct trie_t *root = NULL, **dict = NULL, *parent, *node;
  int ret, dict_size, id = 0, parent_id, i;
  unsigned char c, buffer[1024];
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

  /* get dict size */
  if (fread(&dict_size, sizeof(int), 1, fp_input) != 1) {
    ret = -1;
    goto out;
  }

  /* create dict */
  dict = (struct trie_t **) xmalloc(sizeof(struct trie_t *) * dict_size);
  for (i = 0; i < dict_size; i++)
    dict[i] = NULL;

  /* insert root node */
  root = dict[0] = trie_insert(root, 0, id++);
  if (!root) {
    ret = -1;
    goto out;
  }

  for (;;) {
    /* read lz78 pair */
    if (fread(&parent_id, sizeof(int), 1, fp_input) != 1)
      break;
    fread(&c, sizeof(char), 1, fp_input);

    /* get parent */
    parent = dict[parent_id];

    /* insert new node */
    trie_insert(parent, c, id++);
    node = trie_find(parent, c);
    dict[node->id] = node;

    /* decode dict entry */
    for (node = parent, i = 0; node->parent != NULL; node = node->parent, i++)
      buffer[i] = node->c;

    /* write decoded string */
    for (i = i - 1; i >= 0; i--)
      fwrite(&buffer[i], sizeof(char), 1, fp_output);

    /* write next character */
    if (c != (unsigned char) EOF)
      fwrite(&c, sizeof(char), 1, fp_output);
  }

  ret = 0;
out:
  /* free dictionnary */
  xfree(dict);
  trie_free(root);

  /* close files */
  fclose(fp_input);
  fclose(fp_output);

  return ret;
}
