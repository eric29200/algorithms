/*
 * Huffman encoding = lossless data compression method, working at alphabet level :
 * 1 - parse all file to compute frequency of each character
 * 2 - build huffman tree (min heap) based on frequencies
 *     -> every letter is a leaf in the heap
 *     -> more frequent letters have shortest code
 * 3 - build binary code of every letter
 * 4 - write header in compressed file = every letter with its frequency (so decompressor will be able to rebuild the tree)
 * 5 - encode file = replace each letter with binary code
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../data_structures/heap.h"
#include "../utils/mem.h"

#define NB_CHARACTERS             256
#define BUF_SIZE                  1024

#define huffman_leaf(node)        ((node)->left == NULL && (node)->right == NULL)

/*
 * Huffman node.
 */
struct huff_node_t {
  unsigned char item;
  int freq;
  char huff_code[NB_CHARACTERS / 8];
  struct huff_node_t *left;
  struct huff_node_t *right;
};

/*
 * Compare 2 huffman nodes.
 */
static int huff_node_compare(const void *h1, const void *h2)
{
  return ((struct huff_node_t *) h1)->freq - ((struct huff_node_t *) h2)->freq;
}

/*
 * Create a new huffman node.
 */
static struct huff_node_t *huff_node_create(unsigned char item, int freq)
{
  struct huff_node_t *node;

  node = xmalloc(sizeof(struct huff_node_t));
  node->item = item;
  node->freq = freq;
  node->left = NULL;
  node->right = NULL;
  memset(node->huff_code, 0, sizeof(node->huff_code));

  return node;
}

/*
 * Build huffman tree.
 */
static struct huff_node_t *huffman_tree(int *freq, size_t nb_characters)
{
  struct huff_node_t *left, *right, *top, *node;
  struct heap_t *heap;
  size_t i;

  /* create a heap */
  heap = heap_create(HEAP_MIN, NB_CHARACTERS * 2, huff_node_compare);
  if (!heap)
    return NULL;

  /* build min heap */
  for (i = 0; i < NB_CHARACTERS; i++) {
    if (freq[i]) {
      /* create node */
      node = huff_node_create(i, freq[i]);
      if (!node) {
        heap_free(heap);
        return NULL;
      }

      /* insert node */
      heap_insert(heap, node);
    }
  }

  /* build huffman tree */
  while (heap->size > 1) {
    /* extract 2 minimum values */
    left = heap_min(heap);
    right = heap_min(heap);

    /* build parent node (= left frequency + right frequency)*/
    top = huff_node_create('$', left->freq + right->freq);
    if (!top)
      return NULL;

    /* insert parent node in heap */
    top->left = left;
    top->right = right;
    heap_insert(heap, top);
  }

  return heap_min(heap);
}

/*
 * Build huffman codes.
 */
static void huffman_tree_build_codes(struct huff_node_t *root, char *code, int top)
{
  /* build huffman code on left (encode with a zero) */
  if (root->left) {
    code[top] = '0';
    huffman_tree_build_codes(root->left, code, top + 1);
  }

  /* build huffman code on right (encode with a one) */
  if (root->right) {
    code[top] = '1';
    huffman_tree_build_codes(root->right, code, top + 1);
  }

  /* leaf : create code */
  if (huffman_leaf(root))
    strncpy(root->huff_code, code, top);
}

/*
 * Extract huffman nodes from a tree.
 */
static void huffman_tree_extract_nodes(struct huff_node_t *root, struct huff_node_t **nodes)
{
  if (!root)
    return;

  if (root->left)
    huffman_tree_extract_nodes(root->left, nodes);

  if (root->right)
    huffman_tree_extract_nodes(root->right, nodes);

  if (huffman_leaf(root))
    nodes[(int) root->item] = root;
}

/*
 * Free a huffman tree.
 */
static void huffman_tree_free(struct huff_node_t *root)
{
  struct huff_node_t *left, *right;

  if (!root)
    return;

  /* free root */
  left = root->left;
  right = root->right;
  free(root);

  /* free children */
  huffman_tree_free(left);
  huffman_tree_free(right);
}

/*
 * Compute frequencies.
 */
static void huffman_compute_frequencies(FILE *fp, int *freq, size_t nb_characters)
{
  unsigned char buf[BUF_SIZE];
  size_t len, i;

  /* reset frequencies */
  memset(freq, 0, sizeof(int) * nb_characters);

  /* read all file */
  while (1) {
    len = fread(buf, 1, BUF_SIZE, fp);
    if (len <= 0)
      return;

    /* compute buffer */
    for (i = 0; i < len; i++)
      freq[(int) buf[i]]++;
  }
}

/*
 * Write huffman header.
 */
static void huffman_write_header(FILE *fp, struct huff_node_t **nodes, size_t nb_nodes)
{
  size_t i, n;

  /* compute number of nodes */
  for (i = 0, n = 0; i < nb_nodes; i++)
    if (nodes[i])
      n++;

  /* write number of nodes */
  fwrite(&n, sizeof(int), 1, fp);

  /* write dictionnary */
  for (i = 0; i < nb_nodes; i++) {
    if (nodes[i]) {
      fwrite(&nodes[i]->item, sizeof(char), 1, fp);
      fwrite(&nodes[i]->freq, sizeof(int), 1, fp);
    }
  }
}

/*
 * Read huffman header.
 */
static void huffman_read_header(FILE *fp, int *freq, size_t nb_characters)
{
  int nb_nodes, i, f;
  unsigned char c;

  /* reset frequencies */
  memset(freq, 0, sizeof(int) * nb_characters);

  /* read number of nodes */
  fread(&nb_nodes, sizeof(int), 1, fp);

  /* read header */
  for (i = 0; i < nb_nodes; i++) {
    /* read item and its frequency */
    fread(&c, sizeof(char), 1, fp);
    fread(&f, sizeof(int), 1, fp);

    /* store frequency */
    freq[c] = f;
  }
}

/*
 * Write a huffman buffer to output file (input buffer must be converted to binary buffer).
 */
static void huffman_write_buffer(FILE *fp, char *buf, size_t len)
{
  unsigned char c;
  size_t i, j, n;

  if (len <= 0)
    return;

  for (i = 0; i < len; i += 8) {
    c = 0;

    /* number of bits */
    n = 8;
    if (i + 8 > len)
      n = len - i;

    /* convert string huffman code to binary */
    for (j = 0; j < n; j++)
      if (buf[i + j] == '1')
        c |= 1 << (8 - j - 1);

    /* write binary buffer */
    fwrite(&c, sizeof(char), 1, fp);
  }
}

/*
 * Encode file content with huffman codes.
 */
static void huffman_write_content(FILE *fp_input, FILE *fp_output, struct huff_node_t **nodes)
{
  unsigned char buf_input[BUF_SIZE];
  char buf_output[BUF_SIZE];
  struct huff_node_t *node;
  size_t len, i, j, k;

  /* rewind input file */
  rewind(fp_input);

  /* read all file */
  for (k = 0;;) {
    /* read input file */
    len = fread(buf_input, 1, BUF_SIZE, fp_input);
    if (len <= 0)
      break;

    /* convert characters to huffman codes */
    for (i = 0; i < len; i++) {
      /* get huffman node */
      node = nodes[(int) buf_input[i]];

      /* store huffman code in output buffer */
      for (j = 0; node->huff_code[j]; j++) {
        buf_output[k++] = node->huff_code[j];

        /* output buffer full : write it */
        if (k >= BUF_SIZE) {
          huffman_write_buffer(fp_output, buf_output, k);
          k = 0;
        }
      }
    }
  }

  /* write last buffer */
  huffman_write_buffer(fp_output, buf_output, k);
}

/*
 * Decode huffman file content.
 */
static void huffman_read_content(FILE *fp_input, FILE *fp_output, struct huff_node_t *root)
{
  unsigned char buf_input[BUF_SIZE];
  unsigned char buf_output[BUF_SIZE];
  struct huff_node_t *node;
  size_t len, i, j, k;
  int v;

  for (node = root, k = 0;;) {
    /* read input file */
    len = fread(buf_input, 1, BUF_SIZE, fp_input);
    if (len <= 0)
      break;

    /* decode each character */
    for (i = 0; i < len; i++) {
      /* extract each bit */
      for (j = 0; j < 8; j++) {
        v = (buf_input[i] >> (8 - j - 1)) & 1;

        /* walk through the tree */
        if (v)
          node = node->right;
        else
          node = node->left;

        /* leaf : store code */
        if (huffman_leaf(node)) {
          buf_output[k++] = node->item;
          node = root;

          /* output buffer full : write it */
          if (k >= BUF_SIZE) {
            fwrite(buf_output, 1, k, fp_output);
            k = 0;
          }
        }
      }
    }
  }

  /* write last buffer */
  if (k > 0)
    fwrite(buf_output, 1, k, fp_output);
}

/*
 * Huffman encoding of a file.
 */
int huffman_encode(const char *input_file, const char *output_file)
{
  struct huff_node_t *root, *nodes[NB_CHARACTERS];
  int freq[NB_CHARACTERS], ret;
  FILE *fp_input, *fp_output;
  char code[NB_CHARACTERS];
  size_t i;

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

  /* compute frequencies */
  huffman_compute_frequencies(fp_input, freq, NB_CHARACTERS);

  /* build huffman tree */
  root = huffman_tree(freq, NB_CHARACTERS);
  if (!root) {
    return -1;
    fclose(fp_input);
    fclose(fp_output);
  }

  /* build huffman codes */
  huffman_tree_build_codes(root, code, 0);

  /* extract nodes from tree */
  for (i = 0; i < NB_CHARACTERS; i++)
    nodes[i] = NULL;
  huffman_tree_extract_nodes(root, nodes);

  /*  write header */
  huffman_write_header(fp_output, nodes, NB_CHARACTERS);

  /* write codes */
  huffman_write_content(fp_input, fp_output, nodes);

  /* free huffman tree */
  huffman_tree_free(root);

  /* close files */
  fclose(fp_input);
  fclose(fp_output);

  return 0;
}

/*
 * Huffman decoding of a file.
 */
int huffman_decode(const char *input_file, const char *output_file)
{
  FILE *fp_input, *fp_output;
  struct huff_node_t *root;
  int freq[NB_CHARACTERS];
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

  /* read header */
  huffman_read_header(fp_input, freq, NB_CHARACTERS);

  /* build huffman tree */
  root = huffman_tree(freq, NB_CHARACTERS);
  if (!root) {
    return -1;
    fclose(fp_input);
    fclose(fp_output);
  }

  /* decode file's content */
  huffman_read_content(fp_input, fp_output, root);

  /* free huffman tree */
  huffman_tree_free(root);

  /* close files */
  fclose(fp_input);
  fclose(fp_output);

  return 0;
}
