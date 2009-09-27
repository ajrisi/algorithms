/**
 * Implementation of a min-heap
 */

#ifndef MINHEAP_H_
#define HINHEAP_H_

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

minheap *minheap_init(int max_size,
		      minheap_dupitem_fn dupitem_fn,
		      minheap_orderitem_fn orderitem_fn,
		      minheap_freeitem_fn freeitem_fn);

void *minheap_insert(minheap *mh, void *data);

void * minheap_extract(minheap *mh);

void minheap_free(minheap *mh);


#endif
