/**
 * @file   heap.h
 * @author Adam Risi <ajrisi@gmail.com>
 * @date   Sun Sep 27 00:20:51 2009
 * 
 * @brief  This is an implementation of a statically allocated heap
 * 
 * 
 */


#ifndef HEAP_H_
#define HEAP_H_

typedef void*(*heap_dupitem_fn)(void *);
typedef void(*heap_freeitem_fn)(void *);

/**
 * The order fn works where it returns pos if item 1 should be above
 * item 2, and neg if item 2 should be above item 1
 * 
 */
typedef int(*heap_orderitem_fn)(void *, void*);

typedef struct heap_s heap;
struct heap_s {
  int max_heap_size;
  void **heap_items;
  heap_dupitem_fn dupitem_fn;
  heap_orderitem_fn orderitem_fn;
  heap_freeitem_fn freeitem_fn;
  int next_free_idx;
};


/** 
 * Creates a new heap. If a heap is created with this, then it has
 * to be freed later with heap_free
 * 
 * @param max_size the maximum size of the heap
 * @param dupitem_fn a function that duplicates an item, so that the heap can
 *                   have its own copy. If NULL, then the heap will not make 
 *                   its own copies
 * @param orderitem_fn a function that will be used to
 *                     compare two items, for heap ordering
 * @param freeitem_fn frees an item that was created with the dupitem function 
 * 
 * @return a new heap, or NULL on failure
 */
heap *heap_new(int max_size,
		      heap_dupitem_fn dupitem_fn,
		      heap_orderitem_fn orderitem_fn,
		      heap_freeitem_fn freeitem_fn);

/** 
 * Inserts an item into the heap
 * 
 * @param h the heap to use
 * @param data the address of the item to add
 * 
 * @return a pointer to the added item, or NULL on failure
 */
void *heap_insert(heap *h, void *data);

/** 
 * Extracts an item from the heap. Once you have the item, you should pass it to
 * your heap freeing function (you can use h->freeitem_fn(item)) to prevent
 * memory leaks
 * 
 * @param h the heap to extract from
 * 
 * @return a pointer to the item, or NULL on failure
 */
void *heap_extract(heap *h);

/** 
 * True if the heap is empty
 * 
 * @param h the heap
 * 
 * @return 1 if the heap is empty, otherwise 0
 */
int heap_isempty(heap *h);

/** 
 * True if the heap is full
 * 
 * @param h the heap
 * 
 * @return 1 if the heap is full, otherwise 0
 */
int heap_isfull(heap *h);

/** 
 * This will free the memory associated with a heap
 * 
 * @param h 
 */
void heap_free(heap *h);


#endif
