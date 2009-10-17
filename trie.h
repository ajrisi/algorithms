/**
 * @file   trie.h
 * @author Adam Risi <ajrisi@gmail.com>
 * @date   Sat Oct 17 16:09:29 2009
 * 
 * @brief  A character trie. Great for fast string lookups. 
 * 
 * 
 */

#ifndef _TRIE_H
#define _TRIE_H

#include <stdlib.h>
#include <string.h>

typedef struct trie_node_s trie_node;
struct trie_node_s {
  void *value;
  trie_node *next[256];
};

typedef struct trie_s trie;
struct trie_s {
  trie_node *root_node;
  unsigned int size;
};


/**
 * Create a new trie.
 *
 * @return Pointer to a new trie structure, or NULL on failure
 */
trie *trie_new(void);

/** 
 * Destroy a trie.
 *
 * @param t The trie to destroy.
 */
void trie_free(trie *t);

/** 
 * Insert a string, value pair into the trie. This version asks for
 * the length of the string being inserted, for '\0' containing
 * strings.
 * 
 * @param t the trie 
 * @param key the key to insert
 * @param len the length of the key
 * @param value the value to associate with the key in the trie
 * 
 * @return 0 on success, other on failure
 */
int trie_insert_n(trie *t, char *key, int len, void *value);

/**
 * Insert a new key-value pair into a trie. Can not use NULL as value
 *
 * @param t The trie.
 * @param key The key to access the new value.
 * @param value The value.
 * @return returns -1 on failure, and 0 on success
 */
int trie_insert(trie *t, char *key, void *value);

/** 
 * Look up the value for a key in the trie, this version takes the
 * length of the string
 * 
 * @param t the trie
 * @param key the key to look up
 * @param len the length of the key
 * 
 * @return the value associated with key, or NULL on error/not found
 */
void *trie_lookup_n(trie *t, char *key, unsigned int len);

/**
 * Look up a value from its key in a trie.
 *
 * @param t The trie.
 * @param key The key.
 * @return The value associated with the key, or NULL if not found in the trie.
 */
void *trie_lookup(trie *t, char *key);

/** 
 * Removes an entry from the trie, this one takes the size of the key
 * 
 * @param t the trie
 * @param key the key
 * @param len the length of the key
 */
void trie_remove_n(trie *t, char *key, unsigned int len);

/**
 * Remove an entry from a trie.
 *
 * @param t The trie.
 * @param key The key of the entry to remove.
 */
void trie_remove(trie *t, char *key);

/** 
 * Find the number of entries in a trie.
 *
 * @param trie The trie.
 * @return the number of entries in the trie.
 */
unsigned int trie_size(trie *t);

/** 
 * Execute a function for each item in the trie - will be executed in
 * "alphabetic" order (really, ASCII index of character order)
 * 
 * @param t the trie
 * @param foreach_fn the function to execute for each value. Each
 *        exec. should return 0 for success, or something else to stop
 *        the foreach. First param is the key, second the length, and
 *        third the value
 * 
 * @return 0 on success, other on failure
 */
int trie_foreach(trie *t, int(*foreach_fn)(char *, unsigned int, void*));

#endif
