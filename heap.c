/**
 * @file   heap.c
 * @author Adam Risi <ajrisi@gmail.com>
 * @date   Fri Sep 25 17:36:01 2009
 * 
 * @brief  A heap implementation. More documentation in heap.h
 * 
 * 
 */

#include <stdlib.h>
#include <string.h>

#include "heap.h"

/**
 * Hidden functions, used for the sifting up and down of an item in the heap
 * 
 */
static void sift_up(heap *h, int index);
static void sift_down(heap *h, int index);


heap *heap_new(int max_size,
		      heap_dupitem_fn dupitem_fn,
		      heap_orderitem_fn orderitem_fn,
		      heap_freeitem_fn freeitem_fn)
{

  /*Allocate the space for the new heap, and set it to null */
  heap *h = (heap*)malloc(sizeof(heap));
  if(h == NULL) {
    /* the memory for the heap could not be allocated! */
    return NULL;
  }
  memset((void*)h, 0, sizeof(heap));

  if(max_size <= 0) {
    /* can not allocate a heap with no contents! */
    return NULL;
  }

  h->heap_items = (void**)malloc(sizeof(void*)*max_size+1);
  if(h->heap_items == NULL) {
    /* could not allocate the memory needed for the items in the heap */
    free(h);
    return NULL;
  }

  /* clear the items memory pointers to 0 */
  memset(h->heap_items, 0, sizeof(void*)*max_size+1);

  /* save the max size of the heap */
  h->max_heap_size = max_size;

  /* save a pointer to the dup fn */
  h->dupitem_fn = dupitem_fn;

  /* save a pointer to the ordering function */
  h->orderitem_fn = orderitem_fn;

  /* save a pointer to the free an item function */
  h->freeitem_fn = freeitem_fn;

  /* the next free space in the tree is at index 1 - for calc. purposes */
  h->next_free_idx = 1;

  return h;
}

void *heap_insert(heap *h, void *data)
{
  int insert_index;

  /* make sure that we actually have a tree */
  if(h == NULL) {
    /* we didnt get a tree? */
    return NULL;
  }

  /* check to make sure there is space in the tree */
  if(heap_isfull(h)) {
    /* the heap is already full, return NULL */
    return NULL;
  }

  /* check to see if we are using a dup function, and if we are, then
     make a copy of data, and add the copy of the item to the heap.
     If we are not, then just add data to the heap */
  if(h->dupitem_fn != NULL) {
    data = h->dupitem_fn(data);
    if(data == NULL) {
      /* could not duplicate item */
      return NULL;
    }
  }

  insert_index = h->next_free_idx;

  /* the heap is not full, so go ahead and add the item to
     the next available spot in the tree, update next_free_idx  */
  h->heap_items[insert_index] = data;
  h->next_free_idx++;

  /* call sift up to move the item to its proper location */
  sift_up(h, insert_index);
  
  /* return pointer to the data */
  return data;
}

void *heap_extract(heap *h)
{
  void *minitem;

  /* check to make sure we got a heap */
  if(h == NULL) {
    /* we were not given a heap? */
    return NULL;
  }

  /* check to make sure that the heap isnt empty */
  if(heap_isempty(h)) {
    return NULL;
  }

  /* grab the pointer of the item to return */
  minitem = h->heap_items[1];

  h->next_free_idx--;

  /* swap the last item in the heap into the first items location */
  h->heap_items[1] = h->heap_items[h->next_free_idx];
  h->heap_items[h->next_free_idx] = NULL;
    
  /* now, sift down the item at the top of the heap */
  sift_down(h, 1);

  return minitem;
}

int heap_isempty(heap *h)
{
  return (h->next_free_idx == 1);
}

int heap_isfull(heap *h)
{
  return (h->next_free_idx >= h->max_heap_size);  
}

void heap_free(heap *h)
{
  int i;
  /* iterate over the heap,
     call free for each item (if dup was enabled),
     then free the heap itself */
  if(h->dupitem_fn != NULL) {
    for(i = 1; i < h->next_free_idx; i++) {
      h->freeitem_fn(h->heap_items[i]);
    }
  }

  /* free the items array */
  free(h->heap_items);

  /* free the heap itself */
  free(h);
}

static void sift_up(heap *h, int insert_index)
{
  int parent_index;
  int swap_tmp;
  void *parent_item;
  void *insert_item;

  if(h == NULL) {
    /* no heap */
    return;
  }

  /* check to make sure they dont want to sift the root up */
  if(insert_index == 1) {
    /* they did, and since its not really possible, just return */
    return;
  }

  /* calculate the parent index */
  parent_index = insert_index >> 1;

  /* grab pointers to the location of the parent, and the item
     being inserted */
  parent_item = h->heap_items[parent_index];
  insert_item = h->heap_items[insert_index];

  if(h->orderitem_fn(insert_item, parent_item)) {
    /* swap the item and its parent */
    swap_tmp = insert_index;
    h->heap_items[parent_index] = insert_item;
    h->heap_items[insert_index] = parent_item;
    insert_index = parent_index;
    parent_index = swap_tmp;

    /* we did some work, so sift_up again */
    sift_up(h, insert_index);
  }

}

static void sift_down(heap *h, int insert_index)
{
  void *insert_item;
  int child1_index;
  int child2_index;
  int minchild_index;
  void *minchild_item;

  if(h == NULL) {
    /* no heap */
    return;
  }

  if(insert_index >= h->next_free_idx-1) {
    /* index out of range, or last in heap, so no sift necessary */
    return;
  }

  insert_item = h->heap_items[insert_index];

  /* initialize the child indexes */
  child1_index = insert_index << 1;
  child2_index = child1_index + 1;

  if(child1_index < h->next_free_idx) {
    /* there is a child to compare and possibly swap with */
    if(child2_index < h->next_free_idx) {
      /* find the smallest, if it is less than insert item, then swap */
      if(h->orderitem_fn(h->heap_items[child1_index],
			 h->heap_items[child2_index])) {
	/* child 1 preferred at the top of the heap */
	minchild_index = child1_index;
      } else {
	minchild_index = child2_index;
      }
    } else {
      /* only child1 is valid, so that is possible location */
      minchild_index = child1_index;
    }
  } else {
    /* there were no children to try swapping with, so we are done */
    return;
  }

  minchild_item = h->heap_items[minchild_index];

  /* is the target child better than insert_item? */
  if(h->orderitem_fn(minchild_item, insert_item)) {
    /* the child was less than the parent, so swap the two */
    h->heap_items[insert_index] = minchild_item;
    h->heap_items[minchild_index] = insert_item;
    insert_index = minchild_index;
    /* if we did some work, try to sift down again */
    sift_down(h, insert_index);
  }

}
