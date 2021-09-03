#include <stdio.h>
#include <stdlib.h>

#include "trie.h"

/*
 * Allocate a new trie (node).
 */
static struct trie_t *trie_alloc(unsigned char c, int id)
{
  struct trie_t *trie;

  /* allocate a new trie */
  trie = (struct trie_t *) malloc(sizeof(struct trie_t));
  if (!trie)
    return NULL;

  /* set trie */
  trie->c = c;
  trie->id = id;
  trie->children = NULL;
  trie->next = NULL;

  return trie;
}

/*
 * Insert a node in a trie.
 */
struct trie_t *trie_insert(struct trie_t *root, unsigned char c, int id)
{
  struct trie_t *node;

  /* allocate root if needed */
  if (!root) {
    root = trie_alloc(c, id);
    return root;
  }
  /* check if node already exists */
  for (node = root->children; node != NULL; node = node->next) {
    if (node->c == c)
      return root;

    if (!node->next)
      break;
  }

  /* add new node */
  if (node)
    node->next = trie_alloc(c, id);
  else
    root->children = trie_alloc(c, id);


  return root;
}

/*
 * Free a trie.
 */
void trie_free(struct trie_t *root)
{
  struct trie_t *node;

  if (!root)
    return;

  /* free children */
  for (node = root->children; node != NULL; node = node->next)
    trie_free(node);

  /* free node */
  free(root);
}

/*
 * Find a node in a trie.
 */
struct trie_t *trie_find(struct trie_t *root, unsigned char c)
{
  struct trie_t *node;

  if (!root || !root->children)
    return NULL;

  for (node = root->children; node != NULL; node = node->next)
    if (node->c == c)
      return node;

  return NULL;
}

/*
 * Print a trie.
 */
static void trie_print_level(struct trie_t *root, int level)
{
  struct trie_t *child;
  int i;

  if (!root)
    return;

  /* print node */
  for (i = 0; i < level; i++)
    printf(" ");
  printf("(%c ; %d)\n", root->c, root->id);

  /* print children */
  for (child = root->children; child != NULL; child = child->next)
    trie_print_level(child, level + 2);
}

/*
 * Print a trie.
 */
void trie_print(struct trie_t *root)
{
  trie_print_level(root, 0);
}
