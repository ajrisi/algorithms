/**
 * @file   dlist.h
 * @author Adam Risi <ajrisi@gmail.com>
 * @date   Wed Mar 17 13:12:55 2010
x * 
 * @brief A doubly linked list implementation - part of my algorithms
 * collection
 * 
 * 
 */

#ifndef DLIST_H
#define DLIST_H

typedef void*(*dupfn)(void*);
typedef void(*freefn)(void*);

typedef struct dlist_item_s dlist_item;
struct dlist_item_s {
  dlist_item *prev;
  dlist_item *next;
  void *data;
  int is_sentinel;
};

typedef struct dlist_s dlist;
struct dlist_s {
  dupfn fndup;
  freefn fnfree;
  dlist_item *head;
  dlist_item *tail;
};

/** 
 * Create a new doubly linked list
 * 
 * @param fndup a function to duplicate an item on insertion, or NULL
 *        to disable duplication
 * @param fnfree a function to free an item generated with the
 *        duplication function, or NULL to disable freeing
 * 
 * @return a new doubly linked list, or NULL on failure
 */
dlist *dlist_new(dupfn fndup, freefn fnfree);

/** 
 * Frees the memory associated with a doubly linked list
 * 
 * @param d the doubly linked list to free
 */
void dlist_free(dlist *d);

/** 
 * Gets the head of the doubly linked list.
 * 
 * @param d the doubly linked list to access
 * 
 * @return the head of the list, or NULL if the list is empty, or on
 *         failure
 */
dlist_item *dlist_head(dlist *d);

/** 
 * Gets the tail of the doubly linked list.
 * 
 * @param d the doubly linked list to access
 * 
 * @return the tail ofthe list, or NULL if the list is empty, or on
 *         failure
 */
dlist_item *dlist_tail(dlist *d);

/** 
 * Inserts an item in the doubly linked list before a specified item
 * already in the list.
 * 
 * @param d the doubly linked list to access
 * @param di the item to insert before
 * @param item the item to insert
 * 
 * @return the address of the newly created item in the doubly linked
 *         list, or NULL on failure
 */
dlist_item *dlist_insert_before(dlist *d, dlist_item *di, void *item);

/** 
 * Inserts an item in the doubly linked list after a specified item
 * already in the list.
 * 
 * @param d the doubly linked list to access
 * @param di the item to insert after
 * @param item the item to insert
 * 
 * @return the address of the newly created item in the doubly linked
 *         list, or NULL on failure
 */
dlist_item *dlist_insert_after(dlist *d, dlist_item *di, void *item);

/** 
 * Inserts an item in the doubly linked list before the head (becoming
 * the new head).
 * 
 * @param d the doubly linked list to access
 * @param item the item to insert
 * 
 * @return the address ofthe newly created item in the doubly linked
 *         list, or NULL on failure
 */
dlist_item *dlist_insert_head(dlist *d, void *item);

/** 
 * Inserts an item in the doubly linked list after the tail (becoming
 * the new tail).
 * 
 * @param d the doubly linked list to access
 * @param item the item to insert
 * 
 * @return the address ofthe newly created item in the doubly linked
 *         list, or NULL on failure
 */
dlist_item *dlist_insert_tail(dlist *d, void *item);

/** 
 * Removes an item from the doubly linked list, fixes the head and
 * tail if necessary.
 * 
 * @param d the doubly linked list to access
 * @param di the item to remove
 */
void dlist_remove(dlist *d, dlist_item *di);

/** 
 * Removes the current head from a doubly linked list. Adjusts the
 * head node accordingly.
 * 
 * @param d the doubly linked list to access
 * @param di the item to remove
 */
void dlist_remove_head(dlist *d);

/** 
 * Removes the current tail from a doubly linked list. Adjusts the
 * tail node accordingly.
 * 
 * @param d the doubly linked list to access
 * @param di the item to remove
 */
void dlist_remove_tail(dlist *d);

/** 
 * Gets the item before another item in a doubly linked list.
 * 
 * @param di the item to look before
 * 
 * @return the item before di in the doubly linked list, or NULL on
 *          head or failure
 */
dlist_item *dlist_before(dlist_item *di);

/** 
 * Gets the item after another item in a doubly linked list.
 * 
 * @param di the item to look after
 * 
 * @return the item after di in the doubly linked list, or NULL on
 *         tail or failure
 */
dlist_item *dlist_after(dlist_item *di);

/** 
 * Gets the value of a item in the doubly linked list
 * 
 * @param di list item to access
 * 
 * @return the value of the list item, or NULL on error or sentinel
 */
void *dlist_item_value(dlist_item *di);

#endif

