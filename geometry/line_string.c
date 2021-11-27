#include <stdio.h>

#include "geometry.h"
#include "../utils/mem.h"

/*
 * Create a line string.
 */
struct geometry_t *line_string_create(struct point_t *points, size_t nb_points)
{
  struct geometry_t *geometry;

  if (!points || nb_points <= 1)
    return NULL;

  geometry = geometry_create(GEOMETRY_LINE_STRING);
  geometry->u.line_string.points = points;
  geometry->u.line_string.nb_points = nb_points;

  /* compute envelope */
  envelope_create(geometry);

  return geometry;
}

/*
 * Create a multi line string
 */
struct geometry_t *multi_line_string_create(struct geometry_t **line_strings, size_t nb_line_strings)
{
  struct geometry_t *geometry;

  if (!line_strings || nb_line_strings == 0)
    return NULL;

  geometry = geometry_create(GEOMETRY_MULTI_LINE_STRING);
  geometry->u.multi_line_string.line_strings = line_strings;
  geometry->u.multi_line_string.nb_line_strings = nb_line_strings;

  /* compute envelope */
  envelope_create(geometry);

  return geometry;
}

/*
 * Free a line string.
 */
void line_string_free(struct line_string_t *line_string)
{
  if (!line_string)
    return;

  xfree(line_string->points);
}

/*
 * Free a multi line string.
 */
void multi_line_string_free(struct multi_line_string_t *multi_line_string)
{
  size_t i;

  if (!multi_line_string)
    return;

  for (i = 0; i < multi_line_string->nb_line_strings; i++)
    geometry_free(multi_line_string->line_strings[i]);

  xfree(multi_line_string->line_strings);
}

/*
 * Check if line string ls1 intersects line string ls2.
 */
static int line_string_intersects_line_string(struct line_string_t *ls1, struct line_string_t *ls2)
{
  struct point_t *p1, *p2, *p3, *p4;
  size_t i, j;

  /* foreach ls1 points */
  for (i = 1; i < ls1->nb_points; i++) {
    p1 = &ls1->points[i - 1];
    p2 = &ls1->points[i];

    /* foreach ls2 points */
    for (j = 1; j < ls2->nb_points; j++) {
      p3 = &ls2->points[j - 1];
      p4 = &ls2->points[j];

      /* check inter section */
      if (segment_intersects(p1, p2, p3, p4))
        return 1;
    }
  }

  return 0;
}

/*
 * Check if multi line string mls intersects line string ls.
 */
static int multi_line_string_intersects_line_string(struct multi_line_string_t *mls, struct line_string_t *ls)
{
  size_t i;

  /* foreach ls */
  for (i = 0; i < mls->nb_line_strings; i++)
    if (line_string_intersects_line_string(&mls->line_strings[i]->u.line_string, ls))
      return 1;

  return 0;
}

/*
 * Check if multi line string mls1 intersects multi line string mls2.
 */
static int multi_line_string_intersects_multi_line_string(struct multi_line_string_t *mls1,
                                                          struct multi_line_string_t *mls2)
{
  size_t i, j;

  /* foreach ls */
  for (i = 0; i < mls1->nb_line_strings; i++)
    for (j = 0; j < mls2->nb_line_strings; j++)
      if (line_string_intersects_line_string(&mls1->line_strings[i]->u.line_string,
                                             &mls2->line_strings[j]->u.line_string))
        return 1;

  return 0;
}

/*
 * Check if line string intersects g.
 */
int line_string_intersects(struct geometry_t *line_string, struct geometry_t *g)
{
  if (g->type == GEOMETRY_LINE_STRING)
    return line_string_intersects_line_string(&line_string->u.line_string, &g->u.line_string);
  else if (g->type == GEOMETRY_MULTI_LINE_STRING)
    return multi_line_string_intersects_line_string(&g->u.multi_line_string, &line_string->u.line_string);

  return 0;
}

/*
 * Check if multi line string intersects g.
 */
int multi_line_string_intersects(struct geometry_t *multi_line_string, struct geometry_t *g)
{
  if (g->type == GEOMETRY_LINE_STRING)
    return multi_line_string_intersects_line_string(&multi_line_string->u.multi_line_string, &g->u.line_string);
  else if (g->type == GEOMETRY_MULTI_LINE_STRING)
    return multi_line_string_intersects_multi_line_string(&multi_line_string->u.multi_line_string,
                                                          &g->u.multi_line_string);

  return 0;
}
