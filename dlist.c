#include "dlist.h"
#include <stdlib.h>

dlist *dlist_new(dupfn fndup, freefn fnfree)
{
  /* allocate space for a new, empty doubly linked list. Set values to
     default, return dlist */
  dlist *ret;
  dlist_item *head;
  dlist_item *tail;

  ret = (dlist*)malloc(sizeof(dlist));
  if(ret == NULL) {
    return NULL;
  }

  /* create the head and tail sentinel dlist_items */
  head = (dlist_item*)malloc(sizeof(dlist_item));
  if(head == NULL) {
    free(ret);
    return NULL;
  }
  head->is_sentinel = 1;
  head->prev = NULL;
  head->data = NULL;

  tail = (dlist_item*)malloc(sizeof(dlist_item));
  if(tail == NULL) {
    free(ret);
    free(head);
    return NULL;
  }
  tail->is_sentinel = 1;
  tail->next = NULL;
  tail->data = NULL;

  head->next = tail;
  tail->prev = head;

  ret->fndup = fndup;
  ret->fnfree = fnfree;
  ret->head = head;
  ret->tail = tail;

  return ret;
}

void dlist_free(dlist *d)
{
  /* call dlist_remove_head until dlist_head(d) is NULL, then free the
     dlist structure itself */
  if(d == NULL) {
    return;
  }

  while(dlist_head(d) != NULL) {
    dlist_remove_head(d);
  }

  free(d->head);
  free(d->tail);

  free(d);
}

dlist_item *dlist_head(dlist *d)
{
  /* return the address of the dlist item at the head of the dlist */
  if(d == NULL) {
    return NULL;
  }

  /* we hide the sentinels from the programmer */
  if(d->head->next->is_sentinel == 1) {
    return NULL;
  }

  return d->head->next;
}

dlist_item *dlist_tail(dlist *d)
{
  /* return the address of the dlist_item at the tail of the dlist */
  if(d == NULL) {
    return NULL;
  }

  if(d->tail->prev->is_sentinel == 1) {
    return NULL;
  }

  return d->tail->prev;
}

dlist_item *dlist_insert_before(dlist *d, dlist_item *di, void *item)
{
  /* create new dlist_item for item, set its next to di, its prev to
     di's prev, di's prev to it, and di's prev next to it. if di is
     head, change it to head. if di is null, then insert before the
     head */
  dlist_item *new_item;

  if(d == NULL) {
    return NULL;
  }

  /* if dlist_item is null, then insert before head */
  if(di == NULL) {
    return dlist_insert_head(d, item);
  }

  /* create new dlist_item in memory */
  new_item = (dlist_item*)malloc(sizeof(dlist_item));
  if(new_item == NULL) {
    return NULL;
  }

  /* dup the item if fndup is not null */
  if(d->fndup != NULL) {
    new_item->data = d->fndup(item);
  } else {
    new_item->data = item;
  }

  /* adjust pointers to make the item part of the list */
  new_item->next = di;
  new_item->prev = di->prev;
  new_item->is_sentinel = 0;
  new_item->prev->next = new_item;
  new_item->next->prev = new_item;

  return new_item;
}

dlist_item *dlist_insert_after(dlist *d, dlist_item *di, void *item)
{
  /* create new dlist_item for item, set its next to di, its prev to
     di's prev, di's prev to it, and di's prev next to it. if di is
     head, change it to head. if di is null, then insert before the
     head */
  dlist_item *new_item;

  if(d == NULL) {
    return NULL;
  }

  /* if dlist_item is null, then insert after tail */
  if(di == NULL) {
    return dlist_insert_tail(d, item);
  }

  /* create new dlist_item in memory */
  new_item = (dlist_item*)malloc(sizeof(dlist_item));
  if(new_item == NULL) {
    return NULL;
  }

  /* dup the item if fndup is not null */
  if(d->fndup != NULL) {
    new_item->data = d->fndup(item);
  } else {
    new_item->data = item;
  }

  /* adjust pointers to make the item part of the list */
  new_item->next = di->next;
  new_item->prev = di;
  new_item->is_sentinel = 0;
  new_item->prev->next = new_item;
  new_item->next->prev = new_item;

  return new_item;
}

dlist_item *dlist_insert_head(dlist *d, void *item)
{
  /* new->next = d->head
     d->head = new
     new->prev = null */
  dlist_item *new_item;

  if(d == NULL) {
    return NULL;
  }

  /* create new dlist_item in memory */
  new_item = (dlist_item*)malloc(sizeof(dlist_item));
  if(new_item == NULL) {
    return NULL;
  }

  /* dup the item if fndup is not null */
  if(d->fndup != NULL) {
    new_item->data = d->fndup(item);
  } else {
    new_item->data = item;
  }
     
  new_item->prev = d->head;
  new_item->next = d->head->next;
  new_item->is_sentinel = 0;
  new_item->prev->next = new_item;
  new_item->next->prev = new_item;

  return new_item;
}

dlist_item *dlist_insert_tail(dlist *d, void *item)
{
  dlist_item *new_item;

  if(d == NULL) {
    return NULL;
  }

  /* create new dlist_item in memory */
  new_item = (dlist_item*)malloc(sizeof(dlist_item));
  if(new_item == NULL) {
    return NULL;
  }
  
  /* dup the item if fndup is not null */
  if(d->fndup != NULL) {
    new_item->data = d->fndup(item);
  } else {
    new_item->data = item;
  }
     
  new_item->prev = d->tail->prev;
  new_item->next = d->tail;
  new_item->is_sentinel = 0;
  new_item->prev->next = new_item;
  new_item->next->prev = new_item;

  return new_item;
}

void dlist_remove(dlist *d, dlist_item *di)
{
  /* di-prev-next = di-next
     di-next-prev = di-prev
     freefn
     free */
  if( (d == NULL) ||
      (di == NULL) ||
      (di->is_sentinel == 1) ) {
    return;
  }

  di->next->prev = di->prev;
  di->prev->next = di->next;

  if(d->fnfree != NULL) {
    d->fnfree(di->data);
  }

  free(di);
}

void dlist_remove_head(dlist *d)
{
  dlist_remove(d, dlist_head(d));
}

void dlist_remove_tail(dlist *d)
{
  dlist_remove(d, dlist_tail(d));
}

dlist_item *dlist_before(dlist_item *di)
{
  if(di == NULL) {
    return NULL;
  }

  if(di->prev->is_sentinel == 1) {
    return NULL;
  }

  return di->prev;
}

dlist_item *dlist_after(dlist_item *di)
{
  if(di == NULL) {
    return NULL;
  }
  
  if(di->next->is_sentinel == 1) {
    return NULL;
  }

  return di->next;
}

void *dlist_item_value(dlist_item *di)
{
  if( (di == NULL) ||
      (di->is_sentinel == 1)) {
    return NULL;
  }

  return di->data;
}
