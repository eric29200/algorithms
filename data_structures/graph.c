#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "queue.h"
#include "priority_queue.h"
#include "../utils/mem.h"

#define DISTANCE_INFINITE       (__INT16_MAX__)

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
  vertex->weight = 0;
  vertex->prev = NULL;
  vertex->next = NULL;
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
void graph_add_edge(struct graph_t *graph, size_t src, size_t dst, int weight)
{
  struct graph_edge_t *edge;

  if (!graph || src >= graph->size || dst >= graph->size)
    return;

  /* create new edge */
  edge = (struct graph_edge_t *) xmalloc(sizeof(struct graph_edge_t));
  edge->dst = graph->vertices[dst];
  edge->weight = weight;

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

/*
 * Compare 2 vertices.
 */
static int compare_vertices(const void *a1, const void *a2)
{
  return ((struct graph_vertex_t *) a1)->weight - ((struct graph_vertex_t *) a2)->weight;
}

/*
 * Djikstra algorithm.
 */
void graph_djikstra(struct graph_t *graph, size_t src, size_t dst)
{
  struct graph_vertex_t *vertex, *dst_vertex, *src_vertex;
  struct priority_queue_t *pqueue;
  struct graph_edge_t *edge;
  struct list_t *it;
  size_t i;

  if (!graph || src > graph->size || dst > graph->size)
    return;

  /* unmark all vertices */
  for (i = 0; i < graph->size; i++) {
    graph->vertices[i]->visited = 0;
    graph->vertices[i]->weight = 0;
    graph->vertices[i]->prev = NULL;
    graph->vertices[i]->next = NULL;
  }

  /* create priority queue */
  pqueue = priority_queue_create(graph->size, compare_vertices);

  /* enqueue first vertex */
  graph->vertices[0]->visited = 1;
  priority_queue_push(pqueue, graph->vertices[0]);

  /* loop until priority queue is empty */
  for (vertex = NULL; !priority_queue_is_empty(pqueue);) {
    /* get next vertex */
    vertex = (struct graph_vertex_t *) priority_queue_pop(pqueue);

    /* end of algorithm */
    if (vertex->id == dst)
      break;

    /* add all adjacent vertices (if not already visited) */
    for (it = vertex->edges; it != NULL; it = it->next) {
      edge = (struct graph_edge_t *) it->data;

      /* add adjacent vertex if not visited */
      if (edge->dst->visited == 0) {
        edge->dst->visited = 1;
        edge->dst->weight = vertex->weight + edge->weight;
        edge->dst->prev = vertex;
        priority_queue_push(pqueue, edge->dst);
        continue;
      }

      /* if adjacent vertex is already visited but current path is better, update weight and path */
      if (vertex->weight + edge->weight < edge->dst->weight) {
        edge->dst->weight = vertex->weight + edge->weight;
        edge->dst->prev = vertex;
      }
    }
  }

  /* no solution */
  if (!vertex || vertex->id != dst)
    goto out;

  /* resolve path from src to dst */
  dst_vertex = vertex;
  for (src_vertex = NULL; vertex != NULL; vertex = vertex->prev) {
    vertex->next = src_vertex;
    src_vertex = vertex;
  }

  /* print solution */
  printf("Shortest path from %s to %s = %d : ", src_vertex->label, dst_vertex->label, dst_vertex->weight);
  printf("%s", src_vertex->label);
  for (vertex = src_vertex->next; vertex != NULL; vertex = vertex->next)
    printf(" -> %s", vertex->label);
  printf("\n");

out:
  /* free priority queue */
  priority_queue_free(pqueue);
}

/*
 * Floyd warshall algorithm (compute shortest paths from/to all vertices).
 */
void graph_floyd_warshall(struct graph_t *graph)
{
  struct graph_edge_t *edge;
  int **distances, dist;
  struct list_t *it;
  size_t i, j, k;

  if (!graph || graph->size == 0)
    return;

  /* create and init distances matrix */
  distances = (int **) xmalloc(sizeof(int *) * graph->size);
  for (i = 0; i < graph->size; i++) {
    distances[i] = (int *) xmalloc(sizeof(int) * graph->size);

    /* set distances to infinity */
    for (j = 0; j < graph->size; j++)
      distances[i][j] = DISTANCE_INFINITE;

    /* set distance to himself */
    distances[i][i] = 0;

    /* set distance to neighbours */
    for (it = graph->vertices[i]->edges; it != NULL; it = it->next) {
      edge = (struct graph_edge_t *) it->data;
      distances[i][edge->dst->id] = edge->weight;
    }
  }

  for (i = 0; i < graph->size; i++) {
    for (j = 0; j < graph->size; j++) {
      for (k = 0; k < graph->size; k++) {
        /* update distance if better */
        dist = distances[j][i] == DISTANCE_INFINITE || distances[i][k] == DISTANCE_INFINITE
          ? DISTANCE_INFINITE : distances[j][i] + distances[i][k];
        if (dist < distances[j][k])
          distances[j][k] = dist;
      }
    }
  }


  /* print distances matrix */
  for (i = 0; i < graph->size; i++) {
    for (j = 0; j < graph->size; j++)
      printf(" %d\t|", distances[i][j] == DISTANCE_INFINITE ? -1 : distances[i][j]);

    printf("\n");
  }

  /* free distances matrix */
  for (i = 0; i < graph->size; i++)
    free(distances[i]);
  free(distances);
}
