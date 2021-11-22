#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <stdio.h>
#include "list.h"

struct graph_edge_t {
  struct graph_vertex_t *dst;
};

struct graph_vertex_t {
  int id;
  char *label;
  int visited;
  struct list_t *edges;
};

struct graph_t {
  struct graph_vertex_t **vertices;
  size_t size;
};

struct graph_t *graph_create();
void graph_free(struct graph_t *graph);
void graph_add_vertex(struct graph_t *graph, const char *label);
void graph_add_edge(struct graph_t *graph, size_t src, size_t dst);
void graph_dfs(struct graph_t *graph);
void graph_bfs(struct graph_t *graph);

#endif
