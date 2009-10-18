#include "dictionary.h"

/**
 * Private functions
 * 
 */
static unsigned long di_hash(void *di);
static unsigned long di_rehash(void *di);
static int di_cmp(void *a, void *b);
static void *di_dup(void *di);
static void di_free(void *di);


dictionary *dictionary_new()
{
  return (dictionary*)hashtable_new(di_hash, di_rehash, di_cmp, di_dup, di_free);
}

char *dictionary_set(dictionary *d, char *key, char *value)
{
  dictionary_item di;
  
  di.key = key;
  di.value = value;

  return (char*)hashtable_insert( (hashtable*)d, (void*)&di);
}

char *dictionary_get(dictionary *d, char *key)
{
  dictionary_item di;

  di.key = key;
  di.value = NULL;

  return (char*)hashtable_find( (hashtable*)d, (void*)&di );
}

void dictionary_remove(dictionary *d, char *key)
{
  dictionary_item di;
  
  di.key = key;
  di.value = NULL;
  
  di_free( hashtable_remove( (hashtable*)d, (void*)&di) );
}

void dictionary_free(dictionary *d)
{
  hashtable_free( (hashtable*)d );
}


/**
 * Private functions
 * 
 */

static unsigned long di_hash(void *di)
{
  return hash_string( ((dictionary_item*)di)->key );
}

static unsigned long di_rehash(void *di)
{
  return rehash_string( ((dictionary_item*)di)->key );
}

static int di_cmp(void *a, void *b)
{
  return strcmp( ((dictionary_item*)a)->key, ((dictionary_item*)b)->key );
}

static void *di_dup(void *di)
{
  dictionary_item *d;

  if(di == NULL) {
    return NULL;
  }

  d = calloc(1, sizeof(dictionary_item));
  if(d == NULL) {
    return NULL;
  }

  d->key = strdup( ((dictionary_item*)di)->key );
  d->value = strdup( ((dictionary_item*)di)->value );

  return (void*)d;
}

static void di_free(void *di)
{
  if(di == NULL) {
    return;
  }
  
  free( ((dictionary_item*)di)->key );
  free( ((dictionary_item*)di)->value );
  free(di);
}
