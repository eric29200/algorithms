#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "queue.h"
#include "../utils/mem.h"

/*
 * Create a graph.
 */
struct graph_t *graph_create()
{
  struct graph_t *graph;

  graph = (struct graph_t *) xmalloc(sizeof(struct graph_t));
  graph->vertices = NULL;
  graph->size = 0;

  return graph;
}

/*
 * Free a vertex.
 */
void __graph_vertex_free(struct graph_vertex_t *vertex)
{
  if (!vertex)
    return;

  list_free_full(vertex->edges, free);
  xfree(vertex->label);
  free(vertex);
}

/*
 * Free a graph.
 */
void graph_free(struct graph_t *graph)
{
  size_t i;

  if (!graph)
    return;

  /* free vertices */
  if (graph->vertices) {
    for (i = 0; i < graph->size; i++)
      __graph_vertex_free(graph->vertices[i]);

    free(graph->vertices);
  }

  free(graph);
}

/*
 * Add a vertex to a graph.
 */
void graph_add_vertex(struct graph_t *graph, const char *label)
{
  struct graph_vertex_t *vertex;

  if (!graph)
    return;

  /* create new vertex */
  vertex = (struct graph_vertex_t *) xmalloc(sizeof(struct graph_vertex_t));
  vertex->id = graph->size;
  vertex->label = label ? xstrdup(label) : NULL;
  vertex->visited = 0;
  vertex->edges = NULL;

  /* add it to graph */
  graph->vertices = (struct graph_vertex_t **) xrealloc(graph->vertices,
                                                        sizeof(struct graph_vertex_t *) * (graph->size + 1));
  graph->vertices[graph->size] = vertex;
  graph->size++;
}

/*
 * Add an edge to a graph (= connect 2 vertices.
 */
void graph_add_edge(struct graph_t *graph, size_t src, size_t dst)
{
  struct graph_edge_t *edge;

  if (!graph || src >= graph->size || dst >= graph->size)
    return;

  /* create new edge */
  edge = (struct graph_edge_t *) xmalloc(sizeof(struct graph_edge_t));
  edge->dst = graph->vertices[dst];

  /* add it at the begining of src edges */
  graph->vertices[src]->edges = list_prepend(graph->vertices[src]->edges, edge);
}

/*
 * Recursive depth first search.
 */
static void __graph_dfs(struct graph_vertex_t *vertex)
{
  struct graph_edge_t *edge;
  struct list_t *it;

  /* visit vertex */
  vertex->visited = 1;
  printf("%s\n", vertex->label);

  /* visite edges */
  for (it = vertex->edges; it != NULL; it = it->next) {
    edge = (struct graph_edge_t *) it->data;
    if (edge->dst->visited == 0)
      __graph_dfs(edge->dst);
  }
}

/*
 * Depth First Search.
 */
void graph_dfs(struct graph_t *graph)
{
  size_t i;

  if (!graph || graph->size == 0)
    return;

  /* unmark all vertices */
  for (i = 0; i < graph->size; i++)
    graph->vertices[i]->visited = 0;

  /* for each vertex */
  for (i = 0; i < graph->size; i++)
    if (graph->vertices[i]->visited == 0)
      __graph_dfs(graph->vertices[i]);
}

/*
 * Breadth First Search.
 */
void graph_bfs(struct graph_t *graph)
{
  struct graph_vertex_t *vertex;
  struct graph_edge_t *edge;
  struct queue_t *queue;
  struct list_t *it;
  size_t i;

  if (!graph || graph->size == 0)
    return;

  /* unmark all vertices */
  for (i = 0; i < graph->size; i++)
    graph->vertices[i]->visited = 0;

  /* create a queue and enqeueue first vertex */
  queue = queue_create();
  queue_push_head(queue, graph->vertices[0]);
  graph->vertices[0]->visited = 1;

  /* go through queue */
  while (!queue_is_empty(queue)) {
    /* get next vertex */
    vertex = queue_pop_head(queue);

    /* mark it visited */
    printf("%s\n", vertex->label);

    /* enqueue edges */
    for (it = vertex->edges; it != NULL; it = it->next) {
      edge = (struct graph_edge_t *) it->data;
      if (edge->dst->visited == 0) {
        edge->dst->visited = 1;
        queue_push_tail(queue, edge->dst);
      }
    }
  }

  /* free queue */
  queue_free(queue);
}
