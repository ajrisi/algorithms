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

int graph_add_vertex(graph *g, void *vertex)
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

int graph_add_edge(graph *g, void *edge_data, int vertex_from, int vertex_to)
{
  int edge_index;
  void *edge_data_copy;

  if(g == NULL) {
    return -1;
  }

  /* check to make sure there is a vertex at vertex_from, vertex_to */
  if((graph_get_vertex(g, vertex_from) == NULL) ||
     (graph_get_vertex(g, vertex_to) == NULL)) {
    return -1;
  }

  edge_index = row_col_to_1d(vertex_from, vertex_to, g->max_verticies);

  if(g->dupedge_fn != NULL) {
    edge_data_copy = g->dupedge_fn(edge_data);
  } else {
    edge_data_copy = edge_data;
  }

  g->edges[edge_index] = edge_data_copy;
  
  g->nedges++;
  
  return edge_index;
}

void graph_remove_vertex(graph *g, int vertex)
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
    graph_remove_edge(g, row_col_to_1d(vertex, i, g->max_verticies));
    graph_remove_edge(g, row_col_to_1d(i, vertex, g->max_verticies));
  }

  /* did we manage vertex memory? */
  if(g->freevertex_fn != NULL) {
    /* free the vertex itself */
    g->freevertex_fn(graph_get_vertex(g, vertex));
  }  

  g->verticies[vertex] = NULL;

  g->nverticies--;
}

void graph_remove_edge(graph *g, int edge_index)
{
  void *edge_to_remove;
    
  if(g == NULL) {
    return;
  }
  
  /* make sure that these verticies actually exist */
  if((graph_get_vertex(g, graph_get_edge_src(g, edge_index)) == NULL) ||
     (graph_get_vertex(g, graph_get_edge_dst(g, edge_index)) == NULL)) {
    return;
  }

  edge_to_remove = graph_get_edge(g, edge_index);
  
  if(edge_to_remove == NULL) {
    return;
  }

  if(g->freeedge_fn != NULL) {
    g->freeedge_fn(edge_to_remove);
  }

  g->edges[edge_index] = NULL;

  g->nedges--;
}

void *graph_get_vertex(graph *g, int vertex)
{
  if(g == NULL) {
    return NULL;
  }

  if(vertex >= g->max_verticies) {
    return NULL;
  }

  return g->verticies[vertex];
}

void *graph_get_edge(graph *g, int edge_index)
{
  if(g == NULL) {
    return NULL;
  }

  if((graph_get_vertex(g, graph_get_edge_src(g, edge_index)) == NULL) ||
     (graph_get_vertex(g, graph_get_edge_dst(g, edge_index)) == NULL)) {
    return NULL;
  }

  if(edge_index >= (g->max_verticies)*(g->max_verticies)) {
    return NULL;
  }

  return g->edges[edge_index];
}

int *graph_get_edges(graph *g, int vertex)
{
  int *from_edges;
  int *to_edges;
  int *full_list;
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
  full_list = malloc(sizeof(int)*nedges);
  if(full_list == NULL) {
    return NULL;
  }
  memset(full_list, 0, sizeof(int)*nedges);
  full_idx = 0;

  /* get the outgoing edges, and copy them in */
  i = 0;
  from_edges = graph_get_edges_src(g, vertex);
  while(from_edges[i] != -1) {
    full_list[full_idx++] = from_edges[i++];
  }
  free(from_edges);

  /* get the incoming edges, copy them in */
  i = 0;
  to_edges = graph_get_edges_dst(g, vertex);
  while(to_edges[i] != -1) {
    full_list[full_idx++] = to_edges[i++];
  }
  free(to_edges);

  full_list[full_idx] = -1;

  return full_list;
}

int *graph_get_edges_src(graph *g, int vertex)
{
  int *full_list;
  void *copy_edge;
  int edge_index;
  int nedges;
  int i;
  int full_idx;

  if(g == NULL) {
    return NULL;
  }

  if(vertex >= g->max_verticies) {
    return NULL;
  }

  nedges = graph_nedges_src(g, vertex) + 1;

  /* allocate the space for the list */
  full_list = malloc(sizeof(int)*nedges);
  if(full_list == NULL) {
    return NULL;
  }
  memset(full_list, 0, sizeof(int)*nedges);
  full_idx = 0;

  /* get the from edges, and copy them in */
  for(i = 0; i < g->max_verticies; i++) {
    edge_index = row_col_to_1d(vertex, i, g->max_verticies);
    copy_edge = graph_get_edge(g, edge_index);
    if(copy_edge != NULL) {
      full_list[full_idx++] = edge_index;
    }
  }

  full_list[full_idx] = -1;

  return full_list;
}

int *graph_get_edges_dst(graph *g, int vertex)
{
  int *full_list;
  int *copy_edge;
  int edge_index;
  int nedges;
  int i;
  int full_idx;

  if(g == NULL) {
    return NULL;
  }

  if(vertex >= g->max_verticies) {
    return NULL;
  }

  nedges = graph_nedges_dst(g, vertex) + 1;

  /* allocate the space for the list */
  full_list = malloc(sizeof(int)*nedges);
  if(full_list == NULL) {
    return NULL;
  }
  memset(full_list, 0, sizeof(int)*nedges);
  full_idx = 0;

  /* get the from edges, and copy them in */
  for(i = 0; i < g->max_verticies; i++) {
    edge_index = row_col_to_1d(i, vertex, g->max_verticies);
    copy_edge = graph_get_edge(g, edge_index);
    if(copy_edge != NULL) {
      full_list[full_idx++] = edge_index;
    }
  }

  full_list[full_idx] = -1;

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
  
  total_edges += graph_nedges_src(g, vertex);
  total_edges += graph_nedges_dst(g, vertex);

  return total_edges;
}

int graph_nedges_dst(graph *g, int vertex)
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
	

int graph_nedges_src(graph *g, int vertex)
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

int graph_get_edge_src(graph *g, int edge_index)
{
  if(g == NULL) {
    return -1;
  }
  return (edge_index - graph_get_edge_dst(g, edge_index)) / g->max_verticies;
}

int graph_get_edge_dst(graph *g, int edge_index)
{
  if(g == NULL) {
    return -1;
  }
  return edge_index % g->max_verticies;
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
    graph_remove_vertex(g, i);
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

