/**
 * @file   graph.c
 * @author Adam Risi <ajrisi@gmail.com>
 * @date   Sun Sep 27 05:20:16 2009
 * 
 * @brief An implementation of a directed graph, preallocates based on
 * max number of verticies. This is implemented as a graph adjacency
 * matrix, so n verticies yields a n^2+n memory requirement. This
 * graph is VERY generic, so if you are looking for an integer
 * weighted graph, then use igraph.
 * 
 * 
 */

#ifndef GRAPH_H_
#define GRAPH_H_

typedef void*(*graph_dupedge_fn)(void*);
typedef void*(*graph_dupvertex_fn)(void*);
typedef void*(*graph_freeedge_fn)(void*);
typedef void*(*graph_freevertex_fn)(void*);


typedef struct graph_s graph;
struct graph_s {
  int max_verticies;
  void *edges;
  void *verticies;
  int nedges;
  int nverticies;

  graph_dupedge_fn dupedge_fn;
  graph_dupvertex_fn dupvertex_fn;
  graph_freeedge_fn freeedge_fn;
  graph_freevertex_fn freevertex_fn;
};

/** 
 * This creates a new graph in memory. It manages its own memory,
 * making a copy of each item that is inserted, unless the
 * dup. functions passed in are NULL. If you pass NULL for the dup
 * functions, then you can pass null for the free functions as well. 
 * 
 * @param max_verticies the maximum number of verticies
 * @param dupedge_fn a function to duplicate an edge
 * @param dupvertex_fn a function to duplicate a vertex
 * @param freeedge_fn a function to free an edge
 * @param freevertex_fn a function to free a vertex
 * 
 * @return either a new graph object in memory, or NULL if there was a
 *         problem creating the new graph
 */
graph *graph_new(int max_verticies,
		 graph_dupedge_fn dupedge_fn,
		 graph_dupvertex_fn dupvertex_fn,
		 graph_freeedge_fn freeedge_fn,
		 graph_freevertex_fn freevertex_fn);

/** 
 * Checks to see if the graph's list of verticies is full.
 * 
 * @param g the graph
 * 
 * @return true (1) if full, or false (0) if you can add another
 *         vertex. Returns -1 if an invalid graph was passed in
 */
int graph_isfull(graph *g);

/** 
 * Adds a vertex to the graph. The vertex passed in will be copied if
 * a dup function was provided
 * 
 * @param g a graph
 * @param vertex the vertex to add
 * 
 * @return the integer index of the vertex. This can be used to get
 *         the pointer of the vertex in memory later. Will return -1
 *         on error
 */
int graph_addvertex(graph *g, void *vertex);

/** 
 * Adds an edge to the graph, the edge is copied if a dup function was
 * provided. The edge is added between two vertex indicies.
 * 
 * @param g the graph
 * @param edge the edge to add
 * @param vertex_from the "from" vertex
 * @param vertex_to the "to" vertex
 * 
 * @return the total number of edges in the graph, or -1 on error
 */
int graph_addedge(graph *g, void *edge, int vertex_from, int vertex_to);

/** 
 * Removes a vertex from the graph. This "breaks" any edges to and
 * from this vertex, and free's them
 * 
 * @param g the graph
 * @param vertex the vertex index to remove.
 */
void graph_removevertex(graph *g, int vertex);

/** 
 * Remove an edge from the graph
 * 
 * @param g the graph
 * @param vertex_from the "from" vertex index
 * @param vertex_to the "to" vertex index
 */
void graph_removeedge(graph *g, int vertex_from, int vertex_to);

/** 
 * Gets the address a vertex stored in memory at a certain vertex index
 * 
 * @param g the graph
 * @param vertex the index of the wanted vertex
 * 
 * @return the vertex object
 */
void *graph_getvertex(graph *g, int vertex);

/** 
 * Gets the address of an edge stored in memory between two verticies.
 * 
 * @param g the graph
 * @param vertex_from the "from" vertex
 * @param vertex_to the "to" vertex
 * 
 * @return the edge object
 */
void *graph_getedge(graph *g, int vertex_from, int vertex_to);

/** 
 * Gets a list of edge objects that are connected to this vertex. This
 * is a simple NULL terminated list that should be freed when you are
 * done using it.
 * 
 * @param g the graph
 * @param vertex the vertex index
 * 
 * @return an array of void* edge objects.
 */
void **graph_getedges(graph *g, int vertex);

/** 
 * Gets the edges associated with an vertex index that are "from" that
 * vertex. This is a simple NULL terminated list, that should be freed
 * when you are done using it
 * 
 * @param g the graph
 * @param vertex the vertex index
 * 
 * @return a void* array of edge objects
 */
void **graph_getstartedges(graph *g, int vertex);

/** 
 * Gets the edges associated with an vertex index that are "to" that
 * vertex. This is a simple NULL terminated list, that should be freed
 * when you are done using it.
 * 
 * @param g the graph
 * @param vertex the vertex index
 * 
 * @return a void* array of edge objects
 */
void **graph_getendedges(graph *g, int vertex);

/** 
 * Gets the number of edges that are attached to a vertex
 * 
 * @param g the graph
 * @param vertex the vertex
 * 
 * @return the number of attached edges, of -1 on error
 */
int graph_nedges(graph *g, int vertex);

/** 
 * Get the number of edges that are incoming to a vertex
 * 
 * @param g the graph
 * @param vertex the vertex
 * 
 * @return the number of incoming edges, or -1 on error
 */
int graph_ninedges(graph *g, int vertex);

/** 
 * Get the number of edges that are outgoing from a vertex
 * 
 * @param g the graph
 * @param vertex the vertex
 * 
 * @return the number of outgoing edges, or -1 on error
 */
int graph_noutedges(graph *g, int vertex);

/** 
 * Frees the memory associated with a graph. This includes all edges and verticie
 * 
 * @param g the graph to free
 */
void graph_free(graph *g);


#endif
