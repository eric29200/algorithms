#include <stdio.h>
#include <stdlib.h>

#include "tree.h"
#include "../utils/mem.h"

/*
 * Create a tree.
 */
struct tree_t *tree_create(int (*node_cmp)(const void *, const void *), void (*free_func)(void *))
{
  struct tree_t *tree;

  tree = (struct tree_t *) xmalloc(sizeof(struct tree_t));
  tree->root = NULL;
  tree->node_cmp = node_cmp;
  tree->free_func = free_func;

  return tree;
}

/*
 * Free a node.
 */
static void __tree_node_free(struct tree_t *tree, struct tree_node_t *root)
{
  if (!root)
    return;

  if (tree->free_func)
    tree->free_func(root->data);

  __tree_node_free(tree, root->left);
  __tree_node_free(tree, root->right);
  free(root);
}

/*
 * Free a tree.
 */
void tree_free(struct tree_t *tree)
{
  if (!tree)
    return;

  __tree_node_free(tree, tree->root);
  free(tree);
}

/*
 * Insert a node in a tree.
 */
static struct tree_node_t *__tree_node_insert(struct tree_t *tree, struct tree_node_t *root, void *data)
{
  int res;

  if (!root) {
    root = (struct tree_node_t *) xmalloc(sizeof(struct tree_node_t));
    root->data = data;
    root->left = NULL;
    root->right = NULL;
    return root;
  }

  res = tree->node_cmp(data, root->data);
  if (res < 0)
    root->left = __tree_node_insert(tree, root->left, data);
  else if (res > 0)
    root->right = __tree_node_insert(tree, root->right, data);

  return root;
}

/*
 * Insert an item into a tree.
 */
void tree_insert(struct tree_t *tree, void *data)
{
  if (!tree)
    return;

  tree->root = __tree_node_insert(tree, tree->root, data);
}

/*
 * Find a node in a tree.
 */
static struct tree_node_t *__tree_node_find(struct tree_t *tree, struct tree_node_t *root, void *data)
{
  int res;

  if (!root)
    return NULL;

  res = tree->node_cmp(data, root->data);
  if (res < 0)
    return __tree_node_find(tree, root->left, data);

  if (res > 0)
    return __tree_node_find(tree, root->right, data);

  return root;
}

/*
 * Find an item in a tree.
 */
struct tree_node_t *tree_find(struct tree_t *tree, void *data)
{
  if (!tree || !tree->root)
    return NULL;

  return __tree_node_find(tree, tree->root, data);
}

/*
 * Get minimum node in a sub tree.
 */
struct tree_node_t *__tree_node_min(struct tree_node_t *root)
{
  if (!root)
    return NULL;

  if (!root->left)
    return root;

  return __tree_node_min(root->left);
}

/*
 * Get maximum node in a sub tree.
 */
struct tree_node_t *__tree_node_max(struct tree_node_t *root)
{
  if (!root)
    return NULL;

  if (!root->right)
    return root;

  return __tree_node_max(root->right);
}

/*
 * Delete a node in a tree.
 */
struct tree_node_t *__tree_node_delete(struct tree_t *tree, struct tree_node_t *root, void *data)
{
  struct tree_node_t *tmp;
  int res;

  if (!root)
    return NULL;

  res = tree->node_cmp(data, root->data);
  if (res < 0) {
    root->left = __tree_node_delete(tree, root->left, data);
  } else if (res > 0) {
    root->right = __tree_node_delete(tree, root->right, data);
  } else if (root->left && root->right) {
    tmp = __tree_node_min(root->right);
    root->data = tmp->data;
    root->right = __tree_node_delete(tree, root->right, root->data);
  } else {
    tmp = root;
    if (!root->left)
      root = root->right;
    else
      root = root->left;

    if (tree->free_func)
      tree->free_func(tmp->data);
    free(tmp);
  }

  return root;
}
