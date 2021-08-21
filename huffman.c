#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "heap.h"

#define NB_CHARACTERS             256

#define huffman_leaf(node)        ((node)->left == NULL && (node)->right == NULL)

/*
 * Huffman node.
 */
struct huff_node_t {
  char item;
  int freq;
  int huff_code[NB_CHARACTERS / 8];
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
static void huffman_tree_build_codes(struct huff_node_t *root, int code[], int top)
{
  int i;

  /* build huffman code on left (encode with a zero) */
  if (root->left) {
    code[top] = 0;
    huffman_tree_build_codes(root->left, code, top + 1);
  }

  /* build huffman code on right (encode with a one) */
  if (root->right) {
    code[top] = 1;
    huffman_tree_build_codes(root->right, code, top + 1);
  }

  /* leaf : create code */
  if (huffman_leaf(root))
    for (i = 0; i < top; i++)
      if (code[i])
        root->huff_code[(top - i - 1) / 8] |= 1 << ((top - i - 1) % 8);
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
 * Huffman encoding of a string.
 */
void huffman_encode(const char *s)
{
  struct huff_node_t *root, *nodes[NB_CHARACTERS];
  int freq[NB_CHARACTERS], code[NB_CHARACTERS];
  size_t i;

  /* handle null string */
  if (!s)
    return;

  /* compute frequencies */
  memset(freq, 0, sizeof(int) * NB_CHARACTERS);
  while (*s)
    freq[(int) *s++]++;

  /* build huffman tree */
  root = huffman_tree(freq, NB_CHARACTERS);
  if (!root)
    return;

  /* build huffman codes */
  huffman_tree_build_codes(root, code, 0);

  /* extract nodes from tree */
  for (i = 0; i < NB_CHARACTERS; i++)
    nodes[i] = NULL;
  huffman_tree_extract_nodes(root, nodes);

  /* TODO : write header */

  /* TODO : write codes */

  /* free huffman tree */
  huffman_tree_free(root);
}
