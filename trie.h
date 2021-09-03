#ifndef _TRIE_H_
#define _TRIE_H_

struct trie_t {
  int id;
  unsigned char c;
  struct trie_t *children;
  struct trie_t *next;
};

struct trie_t *trie_insert(struct trie_t *root, unsigned char c, int id);
void trie_free(struct trie_t *root);
struct trie_t *trie_find(struct trie_t *root, unsigned char c);
void trie_print(struct trie_t *root);

#endif
