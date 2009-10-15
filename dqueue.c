#include "dqueue.h"

dqueue *dqueue_new(dqueue_itemdup_fn itemdup_fn, dqueue_itemfree_fn itemfree_fn)
{
  dqueue *q;

  q = (dqueue *)calloc(1, sizeof(dqueue));
  if(q == NULL) {
    return NULL;
  }

  q->size = 0;
  
  q->head = NULL;
  q->tail = NULL;

  q->itemdup_fn = itemdup_fn;
  q->itemfree_fn = itemfree_fn;

  return q;
}

void *dqueue_front(dqueue *q)
{
  if(q == NULL) {
    return NULL;
  }

  return q->head->data;
}


void *dqueue_enqueue(dqueue *q, void *item)
{
  void *insert_item;
  queue_item *oldhead;
  /* an item is enqueued by duplicating it, setting it as the head,
     and setting its next to the old head */
  
  if((q == NULL) ||
     (item == NULL)) {
    return NULL;
  }

  oldhead = q->head;
  
  q->head = (queue_item *)calloc(1, sizeof(queue_item));
  if(q->head == NULL) {
    q->head = oldhead;
    return NULL;
  }

  oldhead->prev = q->head;
  q->head->next = oldhead;
  q->head->prev = NULL;

  if(q->itemdup_fn != NULL) {
    insert_item = q->itemdup_fn(item);
    if(insert_item == NULL) {
      return NULL;
    }
  } else {
    insert_item = item;
  }

  q->head->data = insert_item;

  if(q->size == 0) {
    /* this is the first item, so it is also the tail */
    q->tail = q->head;
  }

  q->size++;

  return insert_item;
}

void *dqueue_dequeue(dqueue *q)
{
  queue_item *oldtail;
  void *oldtail_data;

  if(q == NULL) {
    return NULL;
  }

  if(q->size == 0) {
    return NULL;
  }

  oldtail = q->tail;
  q->tail = oldtail->prev;
  q->tail->next = NULL;

  oldtail_data = oldtail->data;
  
  free(oldtail);
  
  q->size--;

  return oldtail_data;
}

unsigned int dqueue_size(dqueue *q)
{
  if(q == NULL) {
    return 0;
  }

  return q->size;
}

int dqueue_isempty(dqueue *q)
{
  if(q == NULL) {
    return 1;
  }
  return (q->head == NULL);
}

void dqueue_free(dqueue *q)
{
  queue_item *i;

  if(q == NULL) {
    return;
  }


  for(i = q->head; i != NULL; i = i->next) {
    if(q->itemfree_fn != NULL) {
      q->itemfree_fn(i->data);
    }
    if(i == q->tail) {
      free(i);
    } else {
      free(i->prev);
    }
  }
  
  free(q);
}
