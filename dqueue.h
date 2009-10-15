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
  queue_item *next;
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
 * Initialize the dqueue to get a new instance of a dqueue.
 * @param maxsize The maximum size of the dqueue.
 * @return A pointer to a dqueue instance.
 */
dqueue* dqueue_init(int max_size);

/**
 * Insert a new element to the end of the dqueue.
 * @param q The dqueue instance.
 * @param item The item to be added to the dqueue.
 */
void dqueue_insert(dqueue *q, void *item);

/**
 * Remove an element from the front of the dqueue.
 * @param q The dqueue instance.
 */
void* dqueue_remove(dqueue *q);


/**
 * Free the dqueue.
 * @param q The dqueue instance to be freed.
 */
void dqueue_free(dqueue *q);


/**
 * Check if there are elements in the dqueue.
 * @param q The dqueue instance.
 * @return 1 if there are elements in the dqueue, 0 otherwise.
 */
int dqueue_has_elements(dqueue *q);


/**
 * Add a listener to a dqueue.
 * @param q The queoe to attach the listener to.
 * @param listener The listener to be added.
 */
void dqueue_add_listener(dqueue *q, dqueue_notify_listener listener);


/**
 * Remove a listener from a dqueue.
 * @param listener The listener to be removed.
 */
void dqueue_remove_listener(dqueue *q, dqueue_notify_listener listener);


/**
 * Clear all the listeners from a dqueue.
 * @param q The dqueue whose listeners are to be cleared.
 */
void dqueue_clear_listeners(dqueue *q);
