#include <stdio.h>
#include <stdlib.h>

#include "data_structures/graph.h"

/*
 * Graph computation (see algorithms in a nutshell, p. 138).
 */
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
  graph_add_vertex(graph, "9");
  graph_add_vertex(graph, "10");
  graph_add_vertex(graph, "11");
  graph_add_vertex(graph, "12");
  graph_add_vertex(graph, "13");
  graph_add_vertex(graph, "14");
  graph_add_vertex(graph, "15");

  /* create edges */
  graph_add_edge(graph, 0, 1);
  graph_add_edge(graph, 0, 6);
  graph_add_edge(graph, 0, 8);
  graph_add_edge(graph, 1, 0);
  graph_add_edge(graph, 1, 2);
  graph_add_edge(graph, 1, 3);
  graph_add_edge(graph, 2, 1);
  graph_add_edge(graph, 2, 10);
  graph_add_edge(graph, 2, 11);
  graph_add_edge(graph, 3, 1);
  graph_add_edge(graph, 3, 4);
  graph_add_edge(graph, 3, 12);
  graph_add_edge(graph, 4, 3);
  graph_add_edge(graph, 4, 5);
  graph_add_edge(graph, 4, 13);
  graph_add_edge(graph, 5, 4);
  graph_add_edge(graph, 5, 6);
  graph_add_edge(graph, 5, 9);
  graph_add_edge(graph, 6, 0);
  graph_add_edge(graph, 6, 5);
  graph_add_edge(graph, 6, 7);
  graph_add_edge(graph, 7, 6);
  graph_add_edge(graph, 7, 8);
  graph_add_edge(graph, 7, 9);
  graph_add_edge(graph, 8, 0);
  graph_add_edge(graph, 8, 7);
  graph_add_edge(graph, 8, 14);
  graph_add_edge(graph, 9, 5);
  graph_add_edge(graph, 9, 7);
  graph_add_edge(graph, 9, 15);
  graph_add_edge(graph, 10, 2);
  graph_add_edge(graph, 11, 2);
  graph_add_edge(graph, 12, 3);
  graph_add_edge(graph, 13, 4);
  graph_add_edge(graph, 14, 8);
  graph_add_edge(graph, 15, 9);

  /* depth first search */
  graph_dfs(graph);

  /* free graph */
  graph_free(graph);

  return 0;
}
