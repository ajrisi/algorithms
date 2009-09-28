/**
 * @file   graph.c
 * @author Adam Risi <ajrisi@gmail.com>
 * @date   Sun Sep 27 05:20:16 2009
 * 
 * @brief  An implementation of a graph, preallocates based on max size
 * 
 * 
 */

#include <stdlib.h>
#include <string.h>

#include "graph.h"


static int row_col_to_1d(int row, int col, int width);

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

int graph_isfull(graph *g)
{
  if(g == NULL) {
    return -1;
  }

  return (g->nverticies >= g->max_verticies);
}

int graph_addvertex(graph *g, void *vertex)
{
  void *add_vertex;
  void **dest_addr;

  if(g == NULL) {
    return -1;
  }

  if(g->nverticies >= g->max_verticies) {
    return -1;
  }

  if(g->dupvertex_fn != NULL) {
    add_vertex = g->dupvertex_fn(vertex);
  } else {
    add_vertex = vertex;
  }

  dest_addr = g->verticies + g->nverticies;  
  *dest_addr = add_vertex;

  return g->nverticies++;
}

int graph_addedge(graph *g, void *edge, int vertex_from, int vertex_to)
{
  void *add_edge;
  void **dest_addr;

  if(g == NULL) {
    return -1;
  }

  /* check to make sure there is a vertex at vertex_from, vertex_to */
  if(graph_getvertex(g, vertex_from) == NULL) {
    return -1;
  }
  if(graph_getvertex(g, vertex_to) == NULL) {
    return -1;
  }

  if(g->dupedge_fn != NULL) {
    add_edge = g->dupedge_fn(edge);
  } else {
    add_edge = edge;
  }
  
  dest_addr = g->edges + (vertex_from * g->max_verticies) + vertex_to;

  *dest_addr = add_edge;
  
  return g->nedges++;
}

void graph_removevertex(graph *g, int vertex)
{
  int i;

  if(g == NULL) {
    return;
  }

  /* is vertex in scope? */
  if(vertex >= g->max_verticies) {
    return;
  }  

  for(i = 0; i < g->max_verticies; i++) {
    /* remove edges */
    graph_removeedge(g, vertex, i);
    graph_removeedge(g, i ,vertex);
  }

  /* did we manage vertex memory? */
  if(g->dupvertex_fn != NULL) {
    /* free the vertex itself */
    g->freevertex_fn(graph_getvertex(g, vertex));
  }  
  
}

void graph_removeedge(graph *g, int vertex_from, int vertex_to)
{
  void **edge_to_remove;

  edge_to_remove = graph_getedge(g, vertex_from, vertex_to);
  if(*edge_to_remove != NULL) {
    if(g->dupedge_fn != NULL) {
      g->freeedge_fn(*edge_to_remove);
    }
    *edge_to_remove = NULL;
  }
}

void *graph_getvertex(graph *g, int vertex)
{
  void **vertex_to_return;
  
  if(g == NULL) {
    return NULL;
  }

  if(vertex >= g->max_verticies) {
    return NULL;
  }

  vertex_to_return = g->verticies + vertex;

  return *vertex_to_return;
}

void *graph_getedge(graph *g, int vertex_from, int vertex_to)
{
  void **edge_to_return;

  if(g == NULL) {
    return NULL;
  }

  if((vertex_from >= g->max_verticies) ||
     (vertex_to >= g->max_verticies)) {
    return NULL;
  }

  edge_to_return = g->edges + row_col_to_1d(vertex_from, vertex_to, g->max_verticies);
  
  return *edge_to_return;
}

void **graph_getedges(graph *g, int vertex)
{
  void **from_edges;
  void **to_edges;
  void **full_list;
  void *copy_edge;
  int nedges;
  int i;
  int full_idx;

  if(g == NULL) {
    return NULL;
  }

  if(vertex >= g->max_verticies) {
    return NULL;
  }

  nedges = graph_nedges(g, vertex);

  /* allocate the space for the list */
  full_list = malloc(sizeof(void*) * nedges+1);
  if(full_list == NULL) {
    return NULL;
  }
  memset(full_list, 0, sizeof(void*) * nedges+1);
  full_idx = 0;


  /* get the from edges, and copy them in */
  from_edges = graph_getstartedges(g, vertex);
  i = 0;
  copy_edge = *(from_edges);
  while(copy_edge != NULL) {
    *(full_list + full_idx) = copy_edge;
    copy_edge = *(from_edges+1);
    full_idx++;
  }

  to_edges = graph_getendedges(g, vertex);
  i = 0;
  copy_edge = *(to_edges);
  while(copy_edge != NULL) {
    *(full_list + full_idx) = copy_edge;
    copy_edge = *(from_edges+1);
    full_idx++;
  }

  return full_list;
}

void **graph_getstartedges(graph *g, int vertex)
{
  void **full_list;
  void *copy_edge;
  int nedges;
  int i;
  int full_idx;

  if(g == NULL) {
    return NULL;
  }

  if(vertex >= g->max_verticies) {
    return NULL;
  }

  nedges = graph_noutedges(g, vertex);

  /* allocate the space for the list */
  full_list = malloc(sizeof(void*) * nedges+1);
  if(full_list == NULL) {
    return NULL;
  }
  memset(full_list, 0, sizeof(void*) * nedges+1);
  full_idx = 0;


  /* get the from edges, and copy them in */
  for(i = 0; i < g->max_verticies; i++) {
    copy_edge = graph_getedge(g, vertex, i);
    if(copy_edge != NULL) {
      *(full_list + full_idx) = copy_edge;
      full_idx++;
    }
  }

  return full_list;
}

void **graph_getendedges(graph *g, int vertex)
{
  void **full_list;
  void *copy_edge;
  int nedges;
  int i;
  int full_idx;

  if(g == NULL) {
    return NULL;
  }

  if(vertex >= g->max_verticies) {
    return NULL;
  }

  nedges = graph_ninedges(g, vertex);

  /* allocate the space for the list */
  full_list = malloc(sizeof(void*) * nedges+1);
  if(full_list == NULL) {
    return NULL;
  }
  memset(full_list, 0, sizeof(void*) * nedges+1);
  full_idx = 0;


  /* get the from edges, and copy them in */
  for(i = 0; i < g->max_verticies; i++) {
    copy_edge = graph_getedge(g, i, vertex);
    if(copy_edge != NULL) {
      *(full_list + full_idx) = copy_edge;
      full_idx++;
    }
  }

  return full_list;
}

int graph_nedges(graph *g, int vertex)
{
  int total_edges;

  if(g == NULL) {
    return -1;
  }

  if(vertex >= g->max_verticies) {
    return -1;
  }

  total_edges = 0;
  
  total_edges += graph_ninedges(g, vertex);
  total_edges += graph_noutedges(g, vertex);

  return total_edges;
}

int graph_ninedges(graph *g, int vertex)
{
  int i;
  int total_edges;

  if(g == NULL) {
    return -1;
  }

  if(vertex >= g->max_verticies) {
    return -1;
  }

  total_edges = 0;
  
  for(i = 0; i < g->max_verticies; i++) {
    if(graph_getedge(g, i, vertex) != NULL) {
      total_edges++;
    }
  }

  return total_edges;
}
	

int graph_noutedges(graph *g, int vertex)
{
  int i;
  int total_edges;

  if(g == NULL) {
    return -1;
  }

  if(vertex >= g->max_verticies) {
    return -1;
  }

  total_edges = 0;

  for(i = 0; i < g->max_verticies; i++) {
    if(graph_getedge(g, vertex, i) != NULL) {
      total_edges++;
    }
  }

  return total_edges;
}

void graph_free(graph *g)
{
  int i;

  if(g == NULL) {
    return;
  }

  /* for vertex in the graph, call the free vertex routine. This will
     free all of the edges too */
  for(i = 0; i < g->max_verticies; i++) {
    graph_removevertex(g, i);
  }
  
  free(g);

}



/**
 * Private Functions
 * 
 */
static int row_col_to_1d(int row, int col, int width)
{
  return(row*width)+col;
}

