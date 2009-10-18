/**
 * @file   trie.c
 * @author Adam Risi <ajrisi@gmail.com>
 * @date   Sat Oct 17 16:05:46 2009
 * 
 * @brief  A byte (unsigned char) trie, great for fast parsing
 * 
 * 
 */

#include "trie.h"

/**
 * Private functions
 * 
 */
static trie_node *trie_node_new();
static void trie_node_free(trie_node *tn);
static trie_node **lookup_node(trie *t, char *key, unsigned int len);
static int foreach_in(trie_node *n, unsigned int depth, char *s, int(*fn)(char *, unsigned int, void*));

trie *trie_new(void)
{
  trie *t;

  t = (trie*)calloc(1, sizeof(trie));
  if(t == NULL) {
    return NULL;
  }
 
  /* init the root node */
  t->root_node = trie_node_new();
  if(t->root_node == NULL) {
    free(t);
    return NULL;
  }
  
  return t;
}

void trie_free(trie *t)
{
  if(t == NULL) {
    return;
  }
  
  trie_node_free(t->root_node);
  free(t);
}

int trie_insert_n(trie *t, char *key, int len, void *value)
{
  trie_node **n;
  int i;

  if(t == NULL) {
    return -1;
  }

  n = &(t->root_node);

  for(i = 0; i < len; i++) {

    /* move forward a node */
    n = &((*n)->next[ (int)*(key + i) ]);

    /* create the node if it does not exist */
    if(*n == NULL) {
      *n = trie_node_new();
      if(*n == NULL) {
	return -1;
      }
    }

  }

  /* when here, *n is the terminal state for the string */
  if((*n)->value != NULL) {
    t->size--;
  }

  (*n)->value = value;
  t->size++;

  return 0;
}

int trie_insert(trie *t, char *key, void *value)
{
  return trie_insert_n(t, key, strlen(key), value);
}

void *trie_lookup_n(trie *t, char *key, unsigned int len)
{
  int i;
  void *value;
  trie_node *n;

  if(t == NULL) {
    return NULL;
  }
  
  n = t->root_node;
  
  /* walk down the trie, and find an entry */
  for(i = 0; (i < len) && (n != NULL); (n = n->next[ (int)*(key + i) ], i++))
    ;
  
  if(i != len) {
    value = NULL;
  } else {
    value = n->value;
  }
  
  return value;
}

void *trie_lookup(trie *t, char *key)
{
  return trie_lookup_n(t, key, strlen(key));
}

void trie_remove_n(trie *t, char *key, unsigned int len)
{
  int i;
  int allnull;
  trie_node **n;

  allnull = 1;

  if(t == NULL) {
    return;
  }

  /* remove the terminal */
  n = lookup_node(t, key, len); 
  if((n != NULL) &&
     (*n != NULL) &&
     ((*n)->value != NULL)) {
    (*n)->value = NULL;
    t->size--;
  }

  /* cleanup the branch  */
  for(  ;
	((n != NULL) &&
	 (*n != NULL)) ;
	n = lookup_node(t, key, --len)) {
    
    allnull = 1;
    for(i = 0; i < 256; i++) {
      if( (*n)->next[i] != NULL) {
	allnull = 0;
	break;
      }
    }
    
    if(allnull) {
      trie_node_free(*n);
      *n = NULL;
    }
    
  }

}

void trie_remove(trie *t, char *key)
{
  trie_remove_n(t, key, strlen(key));
}

unsigned int trie_size(trie *t)
{
  if(t != NULL) {
    return 0;
  }

  return t->size;
}

int trie_foreach(trie *t, int(*foreach_fn)(char *, unsigned int, void*))
{
  return foreach_in(t->root_node, 0, NULL, foreach_fn);
}

static trie_node *trie_node_new()
{
  trie_node *tn;

  tn = (trie_node*)calloc(1, sizeof(trie_node));
  
  return tn;
}

static void trie_node_free(trie_node *tn)
{
  int i;

  if(tn == NULL) {
    return;
  }

  /* call free on all children nodes, then free self */
  for(i = 0; i < 256; i++) {
    trie_node_free(tn->next[i]);
  }

  free(tn);

}

/* returns the address of the pointer to the terminal node for key
   in the next structure of key-1 */
static trie_node **lookup_node(trie *t, char *key, unsigned int len)
{
  int i;
  trie_node **n;
  
  if(t == NULL) {
    return NULL;
  }
  
  n = &(t->root_node);
  
  /* walk down the trie, and find an entry */
  for(i = 0; (i < len-1) && (*n != NULL); (n = &((*n)->next[ (int)*(key + i) ]), i++))
    ;

  if(i != (len - 1)) {
    return NULL;
  } else {
    return &((*n)->next[ (int)*(key + i) ]);
  }
}

static int foreach_in(trie_node *n, unsigned int depth, char *s, int(*fn)(char *, unsigned int, void*))
{
  int i;
  int ret;
  char *sc;

  if(n == NULL) {
    return 0;
  }

  ret = 0;

  if(n->value != NULL) {
    ret = fn(s, depth, n->value);
    if(ret != 0) {
      return ret;
    }
  }

  sc = (char*)calloc(depth+2, sizeof(char));
  if(sc == NULL) {
    return 1;
  }
  memcpy(sc, s, depth);

  for(i = 0; i < 256; i++) {
    sc[depth] = i;
    ret = foreach_in(n->next[i], depth+1, sc, fn);
    if(ret != 0) {
      break;
    }
  }

  free(sc);

  return ret;
}

