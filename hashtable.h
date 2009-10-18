/**
 * @file   hashtable.h
 * @author Adam Risi <ajrisi@gmail.com>
 * @date   Fri Oct 16 12:16:23 2009
 * 
 * @brief A hash map implementaton in C - originally written by
 * C. B. Falconer
 * 
 * 
 */



#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <stdlib.h>

/* this was "a prime, for easy testing" */
#define HASHTABLE_STARTSIZE 17

/* This is an example of object oriented programming in C, in   */
/* that it isolates the hashtable functioning from the objects  */
/* it stores and retrieves.  It is expected to be useful in     */
/* such areas as symbol tables and Markov chains.               */

/* Note that the user need not be concerned with the table size */

/* This library is intended to be totally re-entrant and thread */
/* safe.  It can provide storage and lookup for arbitrary data  */
/* items of arbitrary types.  The hshkill() function will       */
/* release all associated storage, after which hshinit() is     */
/* needed before using the database again.                      */

/* The pointers returned by hshinsert() and hshfind() may be    */
/* used to modify the data items, PROVIDED THAT such does NOT   */
/* affect the values returned from hshcmp() or hshhash() or     */
/* hshrehash() in any way.  Note that these returned pointers   */
/* are normally NOT the same as the pointers passed in to those */
/* two functions.  This is controlled in hshdupe and hshundupe  */

/* -------------- The auxiliary function types ---------------- */
/* The actual storage for the various void * item pointers is   */
/* known to the calling program, but not to this library. These */
/* function pointers allow the library to adapt itself to       */
/* arbitrary types of data.                                     */
/*                                                              */
/* When called from the hashlib system the 'item' parameter to  */
/* these functions will never be NULL.  The application is thus */
/* free to use such a value for any special purpose, such as    */
/* re-initialization of static variables.                       */

/* a hashfn() returns some hashed value from the item           */
/* Note that two functions of this form must be provided        */
/* The quality of these functions strongly affects performance  */
typedef unsigned long (*hshfn)(void *item);

/* A hshcmpfn() compares two items, and returns -ve, 0 (equal), +ve */
/* corresponding to litem < ritem, litem == ritem, litem > ritem    */
/* It need only return zero/non-zero if not to be used elsewhere    */
typedef int  (*hshcmpfn)(void *litem, void *ritem);

/* A hshdupfn() duplicates the item into malloced space.  Further   */
/* management of the duplicated space is the libraries duty.  It is */
/* only used via hshinsert(), and must return NULL for failure.     */
/* The application is free to modify fields of the allocated space, */
/* provided such modification does NOT affect hshcmpfn              */
typedef void *(*hshdupfn)(void *item);

/* A hshfreefn() reverses the action of a hshdupfn.  It is only     */
/* called during execution of the hshkill() function.  This allows  */
/* clean-up of memory malloced within the hshdupfn. After execution */
/* of hshfree the item pointer should normally not be used.  See    */
/* sort demo for an exception.                                      */
typedef void (*hshfreefn)(void *item);

/* A hshexecfn() performs some operation on a data item.  It may be */
/* passed additional data in datum.  It is only used in walking the */
/* complete stored database. It returns 0 for success.              */
/* xtra will normally be NULL, but may be used for debug purposes   */
/* During a database walk, the item parameter will never be NULL    */
typedef int (*hshexecfn)(void *item, void *datum);

/* ------------ END of auxiliary function types ------------- */

/* Possible error returns, powers of 2 */
typedef enum hsherr_e hsherr;
enum hsherr_e {
  hshOK      = 0,
  hshNOMEM   = 1, 
  hshTBLFULL = 2, 
  hshINTERR  = 4
};

/* NOTE: probes and misses aids evaluating hash functions       */
typedef struct hshstats_s hshstats;
struct hshstats_s {
  unsigned long probes;
  unsigned long misses;
  unsigned long hentries;
  unsigned long hdeleted;
  hsherr herror;
};

/* This is the entity that remembers all about the database  */
/* It occurs in the users data space, keeping the system     */
/* reentrant, because it is passed to all entry routines.    */
typedef struct hashtable_s hashtable;
struct hashtable_s {
  void **htbl;      /* points to an array of void* */
  unsigned long size;          /* size of that array */
  hshfn hash;
  hshfn rehash;
  hshcmpfn cmp;
  hshdupfn dupe;
  hshfreefn undupe;
  hshstats hstatus;
};

/**
 * Public API
 * 
 */

/** 
 * Creates a new hashtable in memory, returns a pointer to it, or NULL
 * on failure
 * 
 * @param hash the hashing function (faster)
 * @param rehash a re-hashing function (slower)
 * @param cmp a comparator function
 * @param dupe a duplication function
 * @param undupe a freeing function
 * @param hdebug TODO: remove this
 * 
 * @return pointer to the hashtable in memory, or NULL on failure
 */
hashtable *hashtable_new(hshfn hash, hshfn rehash,
		     hshcmpfn cmp,
		     hshdupfn dupe, hshfreefn undupe);


/** 
 * Frees the memory associated with a hashtable. Will accept NULL
 * gracefully
 * 
 * @param m the hashtable
 */
void hashtable_free(hashtable *m);

/** 
 * Locates an item in the hashtable, and returns a pointer to it.
 * 
 * @param m the hashtable
 * @param item the item you are looking for (as can be compared
 *              against in the comparator)
 * 
 * @return a pointer to the item in the table, or NULL on failure/not
 *         found
 */
void *hashtable_find(hashtable *m, void *item);

/** 
 * Removes an entry from the hashtable. Returns pointer to the data that
 * was stored in the hash map. By removing from the hash map, you are
 * responsible for freeing the returned memory (created by your
 * dupfn). Normally, you can pass this pointer to your freefn.
 * 
 * @param m the hashtable
 * @param item the item to remove
 * 
 * @return the address of the item in memory, or NULL on failure/not
 *         found
 */
void *hashtable_remove(hashtable *m, void *item);

/** 
 * Insert an item into the hashtable.
 * 
 * @param m the hashtable
 * @param item the item to insert
 * 
 * @return the address of the item in memory, or NULL on failure
 */
void *hashtable_insert(hashtable *m, void *item);

/* 1------------------1 */

/* apply exec to all entries in table. 0 = success */
/* The order of application is arbitrary.  If exec */
/* returns non-zero (error) the walk stops         */
/* datum can provide a global data area for exec   */
/** 
 * Executes exec for each item in the hashtable (no guaranteed order)
 * 
 * @param m the hashtable
 * @param exec an exec fn - return 0 for all successes, and something
 *             else on a failure
 * @param datum data that the exec function will have access to.
 * 
 * @return 0 on success, other on failure
 */
int hashtable_foreach(hashtable *m, hshexecfn exec, void *datum);


/** 
 * Return statistics for this hashtable
 * 
 * @param m the hashtable
 * 
 * @return statistics for the hashtable
 */
hshstats *hashtable_stats(hashtable *m);

/* ============= Useful generic functions ============= */

/** 
 * Generates a hash for a given string - faster algorithm (sdbm)
 * 
 * @param string the string to hash
 * 
 * @return a hash for the string
 */
unsigned long hshstrhash(const char * string);

/** 
 * Re-Hashes a string - the rehash algorithm is allowed to be a little
 * slower. Uses the "rs" string hashing algorithm
 * 
 * @param string the string to hash
 * 
 * @return a hash for the string
 */
unsigned long hshstrehash(const char * string);

#endif /* _HASHTABLE_H_ */

