/**
 * @file   minheap.h
 * @author Adam Risi <ajrisi@gmail.com>
 * @date   Sun Sep 27 00:20:51 2009
 * 
 * @brief  This is an implementation of a statically allocated minheap
 * 
 * 
 */


#ifndef MINHEAP_H_
#define MINHEAP_H_

typedef void*(*minheap_dupitem_fn)(void *);
typedef int(*minheap_orderitem_fn)(void *);
typedef void(*minheap_freeitem_fn)(void *);

typedef struct minheap_s minheap;
struct minheap_s {
  int max_heap_size;
  void **heap_items;
  minheap_dupitem_fn dupitem_fn;
  minheap_orderitem_fn orderitem_fn;
  minheap_freeitem_fn freeitem_fn;
  int next_free_idx;
};


/** 
 * Creates a new minheap. If a heap is created with this, then it has
 * to be freed later with minheap_free
 * 
 * @param max_size the maximum size of the minheap
 * @param dupitem_fn a function that duplicates an item, so that the heap can
 *                   have its own copy. If NULL, then the heap will not make 
 *                   its own copies
 * @param orderitem_fn a function that generates a number for a given item,
 *                     so that it can be compared against other items
 * @param freeitem_fn frees an item that was created with the dupitem function 
 * 
 * @return a new minheap, or NULL on failure
 */
minheap *minheap_new(int max_size,
		      minheap_dupitem_fn dupitem_fn,
		      minheap_orderitem_fn orderitem_fn,
		      minheap_freeitem_fn freeitem_fn);

/** 
 * Inserts an item into the heap
 * 
 * @param mh the heap to use
 * @param data the address of the item to add
 * 
 * @return a pointer to the added item, or NULL on failure
 */
void *minheap_insert(minheap *mh, void *data);

/** 
 * Extracts an item from the heap. Once you have the item, you should pass it to
 * your minheap freeing function (you can use mh->freeitem_fn(item)) to prevent
 * memory leaks
 * 
 * @param mh the heap to extract from
 * 
 * @return a pointer to the item, or NULL on failure
 */
void * minheap_extract(minheap *mh);

/** 
 * This will free the memory associated with a minheap
 * 
 * @param mh 
 */
void minheap_free(minheap *mh);


#endif
