#include <stdlib.h>

#include "slist.h"

/*
 * Free all elements of a slist.
 */
void slist_free(struct slist_t *slist)
{
  struct slist_t *next;

  while (slist) {
    next = slist->next;
    free(slist);
    slist = next;
  }
}

/*
 * Append an element at the end of a slist.
 */
struct slist_t *slist_append(struct slist_t *slist, void *data)
{
  struct slist_t *new_slist;

  /* allocate new element */
  new_slist = (struct slist_t *) malloc(sizeof(struct slist_t));
  if (!new_slist)
    return NULL;

  /* set new element */
  new_slist->data = data;
  new_slist->next = NULL;

  /* append to the slist */
  if (slist)
    slist_last(slist)->next = new_slist;
  else
    slist = new_slist;

  return slist;
}

/*
 * Prepend an element at the beginning of a slist.
 */
struct slist_t *slist_prepend(struct slist_t *slist, void *data)
{
  struct slist_t *new_slist;

  /* allocate new element */
  new_slist = (struct slist_t *) malloc(sizeof(struct slist_t));
  if (!new_slist)
    return NULL;

  /* set new element */
  new_slist->data = data;
  new_slist->next = slist;

  return new_slist;
}

/*
 * Concatenate 2 slists.
 */
struct slist_t *slist_concat(struct slist_t *slist1, struct slist_t *slist2)
{
  if (!slist2)
    return slist1;

  if (slist1) {
    slist_last(slist1)->next = slist2;
    return slist1;
  }

  return slist2;
}

/*
 * Remove an element from a slist (only remove first matching element).
 */
struct slist_t *slist_remove(struct slist_t *slist, void *data)
{
  struct slist_t *prev = NULL, *tmp;

  tmp = slist;
  while (tmp) {
    if (tmp->data == data) {
      if (prev)
        prev->next = tmp->next;
      else
        slist = tmp->next;

      free(tmp);
      break;
    }

    prev = tmp;
    tmp = tmp->next;
  }

  return slist;
}

/*
 * Remove all elements from a slist.
 */
struct slist_t *slist_remove_all(struct slist_t *slist, void *data)
{
  struct slist_t *prev = NULL, *next, *tmp;

  tmp = slist;
  while (tmp) {
    if (tmp->data == data) {
      next = tmp->next;

      if (prev)
        prev->next = next;
      else
        slist = next;

      free(tmp);
      tmp = next;
    } else {
      prev = tmp;
      tmp = tmp->next;
    }
  }

  return slist;

}

/*
 * Copy a slist.
 */
struct slist_t *slist_copy(struct slist_t *slist)
{
  struct slist_t *new_slist = NULL, *new_elt, *prev = NULL;

  for (; slist != NULL; slist = slist->next) {
    new_elt = (struct slist_t *) malloc(sizeof(struct slist_t));
    if (!new_elt) {
      slist_free(new_slist);
      return NULL;
    }

    new_elt->data = slist->data;
    if (prev)
      prev->next = new_elt;
    else
      new_slist = new_elt;

    prev = new_elt;
  }

  return new_slist;
}

/*
 * Find an element in a slist.
 */
struct slist_t *slist_find(struct slist_t *slist, void *data)
{
  while (slist) {
    if (slist->data == data)
      return slist;

    slist = slist->next;
  }

  return NULL;
}

/*
 * Get the last element of a slist.
 */
struct slist_t *slist_last(struct slist_t *slist)
{
  if (!slist)
    return NULL;

  while (slist->next)
    slist = slist->next;

  return slist;
}

/*
 * Get the length of a slist.
 */
size_t slist_length(struct slist_t *slist)
{
  size_t len = 0;

  while (slist) {
    len++;
    slist = slist->next;
  }

  return len;
}
