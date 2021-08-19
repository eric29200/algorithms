#ifndef _SLIST_H_
#define _SLIST_H_

#include <stdio.h>

struct slist_t {
  void *data;
  struct slist_t *next;
};

void slist_free(struct slist_t *slist);
struct slist_t *slist_append(struct slist_t *slist, void *data);
struct slist_t *slist_prepend(struct slist_t *slist, void *data);
struct slist_t *slist_concat(struct slist_t *slist1, struct slist_t *slist2);
struct slist_t *slist_remove(struct slist_t *slist, void *data);
struct slist_t *slist_remove_all(struct slist_t *slist, void *data);
struct slist_t *slist_copy(struct slist_t *slist);
struct slist_t *slist_find(struct slist_t *slist, void *data);
struct slist_t *slist_last(struct slist_t *slist);
size_t slist_length(struct slist_t *slist);

#endif
