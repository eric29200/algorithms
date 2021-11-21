#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
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
  struct graph_edge_t *edge, *next;

  if (!vertex)
    return;

  /* free edges */
  for (edge = vertex->edges; edge != NULL;) {
    next = edge->next;
    free(edge);
    edge = next;
  }

  /* free vertex */
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
  vertex->label = label ? xstrdup(label) : NULL;
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
  edge->next = graph->vertices[src]->edges;
  graph->vertices[src]->edges = edge;
}
