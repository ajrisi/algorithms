#include "dqueue.h"


dqueue *dqueue_new(int maxsize, dqueue_itemdup_fn dup_fn, dqueue_itemfree_fn free_fn)
{
  dqueue *q;

  q = (dqueue *)calloc(1, sizeof(dqueue));
  if(q == NULL) {
    return NULL;
  }

  q->size = 0;
  
  q->head = NULL;
  q->tail = NULL;

  q->dup_fn = dup_fn;
  q->free_fn = free_fn;

  return q;
}

void *dqueue_front(dqueue *q)
{
  if(q == NULL) {
    return NULL;
  }

  return q->head;
}


void *dqueue_enqueue(dequeue *q, void *item)
{

}

void *dqueue_dequeue(dqueue *q)
{

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
    return NULL;
  }
  return (q->head == NULL);
}

void dqueue_free(dqueue *q)
{
  queue_item *i;

  if(q == NULL) {
    return;
  }

  if(q->free_fn != NULL) {
    for(i = q->head; i != NULL; i = i->next) {
      q->free_fn(i->data);
    }
  }

  free(q);
}
