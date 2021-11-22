
#include <stdio.h>
#include <stdlib.h>

#include "data_structures/graph.h"

int main()
{
  struct graph_t *graph;

  /* create graph */
  graph = graph_create();

  /* create vertices */
  graph_add_vertex(graph, "0");
  graph_add_vertex(graph, "1");
  graph_add_vertex(graph, "2");
  graph_add_vertex(graph, "3");
  graph_add_vertex(graph, "4");
  graph_add_vertex(graph, "5");
  graph_add_vertex(graph, "6");
  graph_add_vertex(graph, "7");
  graph_add_vertex(graph, "8");

  /* create edges */
  graph_add_edge(graph, 0, 1, 4);
  graph_add_edge(graph, 0, 7, 8);
  graph_add_edge(graph, 1, 0, 4);
  graph_add_edge(graph, 1, 2, 8);
  graph_add_edge(graph, 1, 7, 11);
  graph_add_edge(graph, 2, 1, 8);
  graph_add_edge(graph, 2, 3, 7);
  graph_add_edge(graph, 2, 5, 4);
  graph_add_edge(graph, 2, 8, 2);
  graph_add_edge(graph, 3, 2, 7);
  graph_add_edge(graph, 3, 4, 9);
  graph_add_edge(graph, 3, 5, 14);
  graph_add_edge(graph, 4, 3, 9);
  graph_add_edge(graph, 4, 5, 10);
  graph_add_edge(graph, 5, 2, 4);
  graph_add_edge(graph, 5, 3, 14);
  graph_add_edge(graph, 5, 4, 10);
  graph_add_edge(graph, 5, 6, 2);
  graph_add_edge(graph, 6, 5, 2);
  graph_add_edge(graph, 6, 7, 1);
  graph_add_edge(graph, 6, 8, 6);
  graph_add_edge(graph, 7, 0, 8);
  graph_add_edge(graph, 7, 1, 11);
  graph_add_edge(graph, 7, 6, 1);
  graph_add_edge(graph, 7, 8, 7);
  graph_add_edge(graph, 8, 2, 2);
  graph_add_edge(graph, 8, 6, 6);
  graph_add_edge(graph, 8, 7, 7);

  /* compute shortest path */
  graph_djikstra(graph, 0, 4);

  /* compute distances matrix */
  graph_floyd_warshall(graph);

  /* free graph */
  graph_free(graph);

  return 0;
}
