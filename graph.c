/**
 * @file   graph.c
 * @author Adam Risi <ajrisi@gmail.com>
 * @date   Sun Sep 27 05:20:16 2009
 * 
 * @brief  An implementation of a graph, preallocates based on max size
 * 
 * 
 */

#include "graph.h"

graph *graph_new(int max_verticies,
		 graph_dupedge_fn dupedge_fn,
		 graph_dupvertex_fn dupvertex_fn,
		 graph_freeedge_fn freeedge_fn,
		 graph_freevertex_fn freevertex_fn)
{
  graph *g = (graph*)malloc(sizeof(graph));
  if(g == NULL) {
    return NULL;
  }
  memset((void*)g, 0, sizeof(graph));


  /* edges */
  g->edges = malloc(sizeof(void*)*max_verticies*max_verticies);
  if(g->edges == NULL) {
    free(g);
    return NULL;
  }
  memset((void*)g->edges, 0, sizeof(void*)*max_verticies*max_verticies);

  /* verticies */
  g->verticies = malloc(sizeof(void*)*max_verticies);
  if(g->verticies == NULL) {
    free(g->edges);
    free(g);
    return NULL;
  }
  memset((void*)g->verticies, 0, sizeof(void*)*max_verticies);

  g->max_verticies = max_verticies;

  g->nedges = 0;
  g->nverticies = 0;


  /* save each of the functions for managing the graph */
  g->dupedge_fn = dupedge_fn;
  g->dupvertex_fn = dupvertex_fn;
  g->freeedge_fn = freeedge_fn;
  g->freevertex_fn = freevertex_fn;

  return g;
}

int graph_addvertex(graph *g, void *vertex)
{

}

int graph_addedge(graph *g, void *edge, int vertex1, int vertex2)
{
}

void graph_removevertex(graph *g, int vertex)
{
}

void graph_removeedge(graph *g, int vertex1, int vertex2)
{
}

void *graph_getvertex(graph *g, int vertex)
{
}

void *graph_getedge(graph *g, int vertex1, int vertex2)
{
}

void **graph_getedges(graph, int vertex)
{
}

void graph_free(graph *g)
{
}

