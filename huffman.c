#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "heap.h"

#define NB_CHARACTERS             256
#define BUF_SIZE                  1024

#define huffman_leaf(node)        ((node)->left == NULL && (node)->right == NULL)

/*
 * Huffman node.
 */
struct huff_node_t {
  char item;
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
static struct huff_node_t *huff_node_create(char item, int freq)
{
  struct huff_node_t *node;

  node = (struct huff_node_t *) malloc(sizeof(struct huff_node_t));
  if (!node)
    return NULL;

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
static struct huff_node_t *huffman_tree(int freq[], size_t nb_characters)
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
static void huffman_tree_build_codes(struct huff_node_t *root, char code[], int top)
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

  /* reset frequences */
  for (i = 0; i < nb_characters; i++)
    freq[i] = 0;

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
  size_t i;

  for (i = 0; i < nb_nodes; i++) {
    if (nodes[i]) {
      fwrite(&nodes[i]->item, sizeof(char), 1, fp);
      fwrite(&nodes[i]->freq, sizeof(int), 1, fp);
    }
  }
}

/*
 * Huffman encoding of a string.
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

  /* TODO : write codes */

  /* free huffman tree */
  huffman_tree_free(root);

  /* close files */
  fclose(fp_input);
  fclose(fp_output);

  return 0;
}
