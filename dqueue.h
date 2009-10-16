/**
 * @file   dqueue.h
 * @author Adam Risi <ajrisi@gmail.com>
 * @date   Thu Oct 15 11:31:55 2009
 * 
 * @brief  This is a simple dqueue implementation
 * 
 * 
 */

#include <stdlib.h>
#include <string.h>

typedef void*(*dqueue_itemdup_fn)(void *);
typedef void(*dqueue_itemfree_fn)(void*);

typedef struct queue_item_s queue_item;
struct queue_item_s {
  void *data;
  queue_item *prev;
};

typedef struct dqueue_s dqueue;
struct dqueue_s
{
  unsigned int size;

  queue_item *head;
  queue_item *tail;
    
  dqueue_itemdup_fn itemdup_fn;
  dqueue_itemfree_fn itemfree_fn;
};

/** 
 * Create a dynamic queue in memory, should be freed when done with
 * (by using dqueue_free).
 * 
 * @param dup_fn a function w/ the prototype void*(*)(void*) that will
 * duplicate an item that goes in the queue, or NULL
 * @param free_fn frees an item in the queue, or NULL
 * 
 * @return a new queue, or NULL on error
 */
dqueue *dqueue_new(dqueue_itemdup_fn itemdup_fn, dqueue_itemfree_fn itemfree_fn);

/** 
 * Get a reference to the front of the queue - you do not own this
 * item, so please, do not free it!
 * 
 * @param q the queue
 * 
 * @return the item at the head of the queue, or NULL on error
 */
void *dqueue_front(dqueue *q);

/** 
 * Enqueues an item into a queue
 * 
 * @param q the queue
 * @param item the item to enqueue at the head of the queue
 * 
 * @return the address of the item on the queue, or NULL on error
 */
void *dqueue_enqueue(dqueue *q, void *item);

/** 
 * Dequeues the item at the end of the queue
 * 
 * @param q the queue
 * 
 * @return the item at the end of the queue, or NULL on error
 */
void *dqueue_dequeue(dqueue *q);

/** 
 * Returns the size of the queue
 * 
 * @param q the queue
 * 
 * @return the size of the queue, or 0 on error
 */
unsigned int dqueue_size(dqueue *q);

/** 
 * Returns true if the queue is empty
 * 
 * @param q the queue
 * 
 * @return 1 on is empty, 0 on is not, and 1 on error
 */
int dqueue_isempty(dqueue *q);

/** 
 * Frees the memory associated with a queue
 * 
 * @param q the queue to free
 */
void dqueue_free(dqueue *q);


