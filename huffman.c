#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "heap.h"

#define NB_CHARACTERS       256

#define huffman_leaf(node)        ((node)->left == NULL && (node)->right == NULL)

/*
 * Huffman node.
 */
struct huff_node_t {
  char item;
  int freq;
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

  return node;
}

/*
 * Build huffman tree.
 */
static struct huff_node_t *huffman_tree(struct heap_t *heap)
{
  struct huff_node_t *left, *right, *top;

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
 * Free a huffman tree.
 */
static void huffman_tree_free(struct huff_node_t *node)
{
  struct huff_node_t *left, *right;

  if (!node)
    return;

  /* free root */
  left = node->left;
  right = node->right;
  free(node);

  /* free children */
  huffman_tree_free(left);
  huffman_tree_free(right);
}

/*
 * Build huffman codes.
 */
static void huffman_codes(struct huff_node_t *root, int code[], int top)
{
  size_t i;

  /* build huffman code on left (encode with a zero) */
  if (root->left) {
    code[top] = 0;
    huffman_codes(root->left, code, top + 1);
  }

  /* build huffman code on right (encode with a one) */
  if (root->right) {
    code[top] = 1;
    huffman_codes(root->right, code, top + 1);
  }

  /* leaf : print code */
  if (huffman_leaf(root)) {
    printf(" %c | ", root->item);

    for (i = 0; i < top; i++)
      printf("%d", code[i]);
    printf("\n");
  }
}

/*
 * Huffman encoding of a string.
 */
void huffman_encode(const char *s)
{
  int freq[NB_CHARACTERS], code[NB_CHARACTERS];
  struct huff_node_t *node, *root = NULL;
  struct heap_t *heap;
  size_t i;

  /* handle null string */
  if (!s)
    return;

  /* memzero frequencies */
  memset(freq, 0, sizeof(int) * NB_CHARACTERS);

  /* create a heap */
  heap = heap_create(HEAP_MIN, NB_CHARACTERS * 2, huff_node_compare);
  if (!heap)
    return;

  /* compute frequencies */
  while (*s)
    freq[(int) *s++]++;

  /* build min heap */
  for (i = 0; i < NB_CHARACTERS; i++) {
    if (freq[i]) {
      /* create node */
      node = huff_node_create(i, freq[i]);
      if (!node)
        goto out;

      /* insert node */
      heap_insert(heap, node);
    }
  }

  /* build huffman tree */
  root = huffman_tree(heap);

  /* build huffman codes */
  huffman_codes(root, code, 0);

out:
  if (root)
    huffman_tree_free(root);
  else
    heap_free_full(heap, free);
}
