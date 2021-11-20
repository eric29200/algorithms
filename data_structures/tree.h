#ifndef _TREE_H_
#define _TREE_H_

struct tree_node_t {
  void *data;
  struct tree_node_t *left;
  struct tree_node_t *right;
};

struct tree_t {
  struct tree_node_t *root;
  int (*node_cmp)(const void *, const void *);
  void (*free_func)(void *);
};

struct tree_t *tree_create(int (*node_cmp)(const void *, const void *), void (*free_func)(void *));
void tree_free(struct tree_t *tree);
void tree_insert(struct tree_t *tree, void *data);
struct tree_node_t *tree_find(struct tree_t *tree, void *data);

#endif
