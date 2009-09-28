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
  g->edges = malloc(sizeof(edge*)*max_verticies*max_verticies);
  if(g->edges == NULL) {
    free(g);
    return NULL;
  }
  memset((void*)g->edges, 0, sizeof(edge*)*max_verticies*max_verticies);

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

  if(g == NULL) {
    return -1;
  }

  if(graph_isfull(g)) {
    return -1;
  }

  if(g->dupvertex_fn != NULL) {
    add_vertex = g->dupvertex_fn(vertex);
  } else {
    add_vertex = vertex;
  }
  
  g->verticies[g->nverticies] = add_vertex;

  return g->nverticies++;
}

int graph_addedge(graph *g, void *edge_data, int vertex_from, int vertex_to)
{
  edge *add_edge;

  if(g == NULL) {
    return -1;
  }

  /* check to make sure there is a vertex at vertex_from, vertex_to */
  if((graph_getvertex(g, vertex_from) == NULL) ||
     (graph_getvertex(g, vertex_to) == NULL)) {
    return -1;
  }

  /* allocate some space for the edge object */
  add_edge = (edge*)malloc(sizeof(edge));
  if(add_edge == NULL) {
    return -1;
  }
  memset(add_edge, 0, sizeof(edge));

  add_edge->vertex_from = vertex_from;
  add_edge->vertex_to = vertex_to;
  add_edge->index = row_col_to_1d(vertex_from, vertex_to, g->max_verticies);

  if(g->dupedge_fn != NULL) {
    add_edge->data = g->dupedge_fn(edge_data);
  } else {
    add_edge->data = edge_data;
  }

  g->edges[add_edge->index] = add_edge;
  
  g->nedges++;
  
  return add_edge->index;
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

  if(g->verticies[vertex] == NULL) {
    return;
  }

  for(i = 0; i < g->max_verticies; i++) {
    /* remove the outgoing, then the incoming */
    graph_removeedge(g, vertex, i);
    graph_removeedge(g, i, vertex);
  }

  /* did we manage vertex memory? */
  if(g->freevertex_fn != NULL) {
    /* free the vertex itself */
    g->freevertex_fn(graph_getvertex(g, vertex));
  }  

  g->verticies[vertex] = NULL;

  g->nverticies--;
}

void graph_removeedge(graph *g, int vertex_from, int vertex_to)
{
  edge *edge_to_remove;

  if(g == NULL) {
    return;
  }

  /* make sure that these verticies actually exist */
  if((graph_getvertex(g, vertex_from) == NULL) ||
     (graph_getvertex(g, vertex_to) == NULL)) {
    return;
  }

  edge_to_remove = graph_getedge(g, vertex_from, vertex_to);
  
  if(edge_to_remove == NULL) {
    return;
  }

  if(g->freeedge_fn != NULL) {
    g->freeedge_fn(edge_to_remove->data);
  }

  g->edges[edge_to_remove->index] = NULL;
  free(edge_to_remove);

  g->nedges--;
}

void *graph_getvertex(graph *g, int vertex)
{
  if(g == NULL) {
    return NULL;
  }

  if(vertex >= g->max_verticies) {
    return NULL;
  }

  return g->verticies[vertex];
}

edge *graph_getedge(graph *g, int vertex_from, int vertex_to)
{
  int edge_index;

  if(g == NULL) {
    return NULL;
  }

  if((graph_getvertex(g, vertex_from) == NULL) ||
     (graph_getvertex(g, vertex_to) == NULL)) {
    return NULL;
  }

  edge_index = row_col_to_1d(vertex_from, vertex_to, g->max_verticies);

  return g->edges[edge_index];
}

edge **graph_getedges(graph *g, int vertex)
{
  edge **from_edges;
  edge **to_edges;
  edge **full_list;
  int nedges;
  int i;
  int full_idx;

  if(g == NULL) {
    return NULL;
  }

  if(vertex >= g->max_verticies) {
    return NULL;
  }

  nedges = graph_nedges(g, vertex) + 1;

  /* allocate the space for the list */
  full_list = malloc(sizeof(edge*)*nedges);
  if(full_list == NULL) {
    return NULL;
  }
  memset(full_list, 0, sizeof(edge*)*nedges);
  full_idx = 0;

  /* get the outgoing edges, and copy them in */
  from_edges = graph_getoutedges(g, vertex);
  i = 0;
  while(from_edges[i] != NULL) {
    full_list[full_idx++] = from_edges[i++];
  }
  free(from_edges);

  /* get the incoming edges, copy them in */
  to_edges = graph_getinedges(g, vertex);
  i = 0;
  while(to_edges[i] != NULL) {
    full_list[full_idx++] = to_edges[i++];
  }
  free(to_edges);

  return full_list;
}

edge **graph_getoutedges(graph *g, int vertex)
{
  edge **full_list;
  edge *copy_edge;
  int nedges;
  int i;
  int full_idx;

  if(g == NULL) {
    return NULL;
  }

  if(vertex >= g->max_verticies) {
    return NULL;
  }

  /* add 1 so that we can return a NULL at the end of the list */
  nedges = graph_noutedges(g, vertex) + 1;

  /* allocate the space for the list */
  full_list = malloc(sizeof(edge*)*nedges);
  if(full_list == NULL) {
    return NULL;
  }
  memset(full_list, 0, sizeof(edge*)*nedges);
  full_idx = 0;


  /* get the from edges, and copy them in */
  for(i = 0; i < g->max_verticies; i++) {
    copy_edge = graph_getedge(g, vertex, i);
    if(copy_edge != NULL) {
      full_list[full_idx++] = copy_edge;
    }
  }

  return full_list;
}

edge **graph_getinedges(graph *g, int vertex)
{
  edge **full_list;
  edge *copy_edge;
  int nedges;
  int i;
  int full_idx;

  if(g == NULL) {
    return NULL;
  }

  if(vertex >= g->max_verticies) {
    return NULL;
  }

  nedges = graph_ninedges(g, vertex) + 1;

  /* allocate the space for the list */
  full_list = malloc(sizeof(edge*) * nedges);
  if(full_list == NULL) {
    return NULL;
  }
  memset(full_list, 0, sizeof(edge*) * nedges);
  full_idx = 0;

  /* get the from edges, and copy them in */
  for(i = 0; i < g->max_verticies; i++) {
    copy_edge = graph_getedge(g, i, vertex);
    if(copy_edge != NULL) {
      full_list[full_idx++] = copy_edge;
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
  int edge_index;
  int total_edges;

  if(g == NULL) {
    return -1;
  }

  if(vertex >= g->max_verticies) {
    return -1;
  }

  total_edges = 0;
  
  for(i = 0; i < g->max_verticies; i++) {
    edge_index = row_col_to_1d(i, vertex, g->max_verticies);
    if(g->edges[edge_index] != NULL) {
      total_edges++;
    }
  }

  return total_edges;
}
	

int graph_noutedges(graph *g, int vertex)
{
  int i;
  int edge_index;
  int total_edges;

  if(g == NULL) {
    return -1;
  }

  if(vertex >= g->max_verticies) {
    return -1;
  }

  total_edges = 0;
  
  for(i = 0; i < g->max_verticies; i++) {
    edge_index = row_col_to_1d(vertex, i, g->max_verticies);
    if(g->edges[edge_index] != NULL) {
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

  free(g->edges);
  free(g->verticies);
  
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

