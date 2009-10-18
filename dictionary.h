/**
 * @file   dictionary.h
 * @author Adam Risi <ajrisi@gmail.com>
 * @date   Sun Oct 18 18:50:32 2009
 * 
 * @brief A dictionary (string -> string) implementation, uses the
 * hashtable structure
 * 
 * 
 */

#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

#include <string.h>

#include "hash.h"
#include "hashtable.h"

typedef hashtable dictionary;

typedef struct dictionary_item_s dictionary_item;
struct dictionary_item_s {
  char *key;
  char *value;
};


dictionary *dictionary_new();

char *dictionary_set(dictionary *d, char *key, char *value);

char *dictionary_get(dictionary *d, char *key);

void dictionary_remove(dictionary *d, char *key);

void dictionary_free(dictionary *d);

#endif /* _DICTIONARY_H_ */
