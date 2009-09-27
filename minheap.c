/**
 * @file   minheap.c
 * @author Adam Risi <ajrisi@gmail.com>
 * @date   Fri Sep 25 17:36:01 2009
 * 
 * @brief  A simple minheap implementation
 * 
 * 
 */

#include <stdlib.h>
#include <string.h>

#include "minheap.h"

/**
 * Hidden functions, used for the sifting up and down of an item in the heap
 * 
 */
static void sift_up(minheap *mh, int index);
static void sift_down(minheap *mh, int index);


minheap *minheap_init(int max_size,
		      minheap_dupitem_fn dupitem_fn,
		      minheap_orderitem_fn orderitem_fn,
		      minheap_freeitem_fn freeitem_fn)
{

  /*Allocate the space for the new minheap, and set it to null */
  minheap *mh = (minheap*)malloc(sizeof(minheap));
  if(mh == NULL) {
    /* the memory for the minheap could not be allocated! */
    return NULL;
  }
  memset((void*)mh, 0, sizeof(minheap));

  if(max_size <= 0) {
    /* can not allocate a heap with no contents! */
    return NULL;
  }

  mh->heap_items = (void**)malloc(sizeof(void*)*max_size+1);
  if(mh->heap_items == NULL) {
    /* could not allocate the memory needed for the items in the heap */
    free(mh);
    return NULL;
  }

  /* clear the items memory pointers to 0 */
  memset(mh->heap_items, 0, sizeof(void*)*max_size+1);

  /* save the max size of the heap */
  mh->max_heap_size = max_size;

  /* save a pointer to the dup fn */
  mh->dupitem_fn = dupitem_fn;

  /* save a pointer to the ordering function */
  mh->orderitem_fn = orderitem_fn;

  /* save a pointer to the free an item function */
  mh->freeitem_fn = freeitem_fn;

  /* the next free space in the tree is at index 1 - for calc. purposes */
  mh->next_free_idx = 1;

  return mh;
}

void *minheap_insert(minheap *mh, void *data)
{
  int insert_index;

  /* make sure that we actually have a tree */
  if(mh == NULL) {
    /* we didnt get a tree? */
    return NULL;
  }

  /* check to make sure there is space in the tree */
  if(mh->next_free_idx >= mh->max_heap_size) {
    /* the heap is already full, return NULL */
    return NULL;
  }

  /* check to see if we are using a dup function, and if we are, then
     make a copy of data, and add the copy of the item to the heap.
     If we are not, then just add data to the heap */
  if(mh->dupitem_fn != NULL) {
    data = mh->dupitem_fn(data);
    if(data == NULL) {
      /* could not duplicate item */
      return NULL;
    }
  }

  insert_index = mh->next_free_idx;

  /* the heap is not full, so go ahead and add the item to
     the next available spot in the tree, update next_free_idx  */
  mh->heap_items[insert_index] = data;
  mh->next_free_idx++;

  /* call sift up to move the item to its proper location */
  sift_up(mh, insert_index);
  
  /* return pointer to the data */
  return data;
}

void *minheap_extract(minheap *mh)
{
  void *minitem;

  /* check to make sure we got a heap */
  if(mh == NULL) {
    /* we were not given a heap? */
    return NULL;
  }

  /* check to make sure that the heap isnt empty */
  if(mh->next_free_idx == 1) {
    /* the heap was empty, return NULL */
    return NULL;
  }

  /* grab the pointer of the item to return */
  minitem = mh->heap_items[1];

  mh->next_free_idx--;

  /* swap the last item in the heap into the first items location */
  mh->heap_items[1] = mh->heap_items[mh->next_free_idx];
  mh->heap_items[mh->next_free_idx] = NULL;
    
  /* now, sift down the item at the top of the heap */
  sift_down(mh, 1);

  return minitem;
}

void minheap_free(minheap *mh)
{
  int i;
  /* iterate over the minheap,
     call free for each item (if dup was enabled),
     then free the heap itself */
  if(mh->dupitem_fn != NULL) {
    for(i = 1; i < mh->next_free_idx; i++) {
      mh->freeitem_fn(mh->heap_items[i]);
    }
  }

  /* free the items array */
  free(mh->heap_items);

  /* free the minheap itself */
  free(mh);
}

static void sift_up(minheap *mh, int insert_index)
{
  int parent_index;
  int swap_tmp;
  void *parent_item;
  void *insert_item;

  if(mh == NULL) {
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
  parent_item = mh->heap_items[parent_index];
  insert_item = mh->heap_items[insert_index];

  if(mh->orderitem_fn(insert_item) < mh->orderitem_fn(parent_item)) {
    /* swap the item and its parent */
    swap_tmp = insert_index;
    mh->heap_items[parent_index] = insert_item;
    mh->heap_items[insert_index] = parent_item;
    insert_index = parent_index;
    parent_index = swap_tmp;

    /* we did some work, so sift_up again */
    sift_up(mh, insert_index);
  }

}

static void sift_down(minheap *mh, int insert_index)
{
  void *insert_item;
  int child1_index;
  int child2_index;
  int minchild_index;
  void *minchild_item;

  if(mh == NULL) {
    /* no heap */
    return;
  }

  if(insert_index >= mh->next_free_idx-1) {
    /* index out of range, or last in heap, so no sift necessary */
    return;
  }

  insert_item = mh->heap_items[insert_index];

  /* initialize the child indexes */
  child1_index = insert_index << 1;
  child2_index = child1_index + 1;

  if(child1_index < mh->next_free_idx) {
    /* there is a child to compare and possibly swap with */
    if(child2_index < mh->next_free_idx) {
      /* find the smallest, if it is less than insert item, then swap */
      if(mh->orderitem_fn(mh->heap_items[child1_index]) < 
	 mh->orderitem_fn(mh->heap_items[child2_index])) {
	/* child 1 was smaller, so it is the new swap index */
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

  minchild_item = mh->heap_items[minchild_index];

  /* is the target child smaller than insert_item? */
  if(mh->orderitem_fn(minchild_item) < mh->orderitem_fn(insert_item)) {
    /* the child was less than the parent, so swap the two */
    mh->heap_items[insert_index] = minchild_item;
    mh->heap_items[minchild_index] = insert_item;
    insert_index = minchild_index;
    /* if we did some work, try to sift down again */
    sift_down(mh, insert_index);
  }

}
