/**
 * @file   graph.c
 * @author Adam Risi <ajrisi@gmail.com>
 * @date   Sun Sep 27 05:20:16 2009
 * 
 * @brief An implementation of a graph, preallocates based on max
 * number of verticies. This is implemented as a graph adjacency
 * matrix, so n verticies yields a n^2+n memory requirement. This
 * graph is VERY generic, so if you are looking for an integer
 * weighted graph, then use igraph.
 * 
 * 
 */

#ifndef GRAPH_H_
#define GRAPH_H_

/* something for the edges, and something for the verticies */
/* should we store them in the graph, or not? We should be able to store in graph */

typedef void*(*graph_dupedge_fn)(void*);
typedef void*(*graph_dupvertex_fn)(void*);
typedef void*(*graph_freeedge_fn)(void*);
typedef void*(*graph_freevertex_fn)(void*);


typedef struct graph_s graph;
struct graph_s {
  int max_verticies;
  void *edges[][];
  void *verticies[];
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
 * Adds a vertex to the graph. The vertex passed in will be copied if
 * a dup function was provided
 * 
 * @param g a graph
 * @param vertex the vertex to add
 * 
 * @return the integer index of the vertex. This can be used to get
 *         the pointer of the vertex in memory later
 */
int graph_addvertex(graph *g, void *vertex);

/** 
 * Adds an edge to the graph, the edge is copied if a dup function was
 * provided. The edge is added between two vertex indicies.
 * 
 * @param g the graph
 * @param edge the edge to add
 * @param vertex1 the "from" vertex
 * @param vertex2 the "to" vertex
 * 
 * @return 
 */
int graph_addedge(graph *g, void *edge, int vertex1, int vertex2);

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
 * @param vertex1 the "from" vertex index
 * @param vertex2 the "to" vertex index
 */
void graph_removeedge(graph *g, int vertex1, int vertex2);

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
 * @param vertex1 the "from" vertex
 * @param vertex2 the "to" vertex
 * 
 * @return the edge object
 */
void *graph_getedge(graph *g, int vertex1, int vertex2);

/** 
 * Gets a list of edge objects that are connected to this vertex. 
 * 
 * @param g the graph
 * @param vertex the vertex index
 * 
 * @return an array of void* edge objects.
 */
void **graph_getedges(graph *g, int vertex);

/** 
 * Gets the edges associated with an vertex index that are "from" that vertex
 * 
 * @param g the graph
 * @param vertex the vertex index
 * 
 * @return a void* array of edge objects
 */
void **graph_getstartedges(graph *g, int vertex);

/** 
 * Gets the edges associated with an vertex index that are "to" that vertex
 * 
 * @param g the graph
 * @param vertex the vertex index
 * 
 * @return a void* array of edge objects
 */
void **graph_getendedges(graph *g, int vertex);

/** 
 * Frees the memory associated with a graph. This includes all edges and verticies
 * 
 * @param g 
 */
void graph_free(graph *g);


#endif
