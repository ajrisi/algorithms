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

  if(q->head == NULL) {
    return NULL;
  }

  return q->tail->data;
}


void *dqueue_enqueue(dqueue *q, void *item)
{
  queue_item *new_item;
  /* an item is enqueued by duplicating it, setting it as the head,
     and setting its next to the old head */
  
  if((q == NULL) ||
     (item == NULL)) {
    return NULL;
  }

  new_item = (queue_item *)calloc(1, sizeof(queue_item));
  if(new_item == NULL) {
    return NULL;
  }

  /* init the new item */
  if(q->itemdup_fn != NULL) {
    new_item->data = q->itemdup_fn(item);
  } else {
    new_item->data = item;
  }

  new_item->prev = NULL;

  if(q->head != NULL) {
    q->head->prev = new_item;
    q->head = new_item;
  } else {
    /* if the head is null, the queue is empty */
    q->head = new_item;
    q->tail = q->head;
  }

  q->size++;

  return new_item->data;
}

void *dqueue_dequeue(dqueue *q)
{
  queue_item *oldtail;
  void *oldtail_data;

  if(q == NULL) {
    return NULL;
  }

  if(q->tail == NULL) {
    return NULL;
  }

  oldtail = q->tail;
  q->tail = oldtail->prev;

  oldtail_data = oldtail->data;
  
  free(oldtail);
  
  q->size--;

  if(q->tail == NULL) {
    q->head = NULL;
  }

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
  return (q->size == 0);
}

void dqueue_free(dqueue *q)
{
  void *tmp;

  if(q == NULL) {
    return;
  }
  
  while(!dqueue_isempty(q)) {
    tmp = dqueue_dequeue(q);
    if((q->itemfree_fn != NULL) &&
       (tmp != NULL)) {
      q->itemfree_fn(tmp);
    }
  }
  
  free(q);
}

void *dupint(void *i)
{
  int *ic;
  ic = malloc(sizeof(int));
  *ic = *(int*)i;
  return (void*)ic;
}


