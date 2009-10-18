#include "hashtable.h"

/**
 * Private functions
 * 
 */
static unsigned long ithprime(size_t i);
static void *inserted(hashtable *master, unsigned long h, void *item, int copying);
static void *putintbl(hashtable *master, void *item, int copying);
static int reorganize(hashtable *master);
static int found(hashtable *master, unsigned long h, void *item);
static unsigned long huntup(hashtable *master, void *item);

/* Threshold above which reorganization is desirable */
#define TTHRESH(sz) (sz - (sz >> 3))

/* Space available before reaching threshold */
/* Ensure this can return a negative value   */
#define TSPACE(m)  ((long)TTHRESH(m->size)		\
		    - (long)m->hstatus.hentries)

/* table of k where 2**n - k is prime, for n=8 up. 0 ends   */
/* These numbers are chosen so that memory allocation will  */
/* usually allow space for system overhead in a 2**n block  */
/*http://www.utm.edu/research/primes/lists/2small/0bit.html */
#define FIRSTN 8
static int primetbl[] = {45, 45, 41, 45, 45, 45, 45, 49,
                         57, 49, 41, 45, 59, 55, 57, 61,
                         63, 61, 45, 79, 0};

hashtable *hashtable_new(hshfn hash, hshfn rehash,
			 hshcmpfn cmp,
			 hshdupfn dupe, hshfreefn undupe)
{
  hashtable *master;
  
  if((hash == NULL) ||
     (rehash == NULL) ||
     (cmp == NULL)) {
    return NULL;
  }
  
  master = calloc(1, sizeof(*master));
  if(master == NULL) {
    return NULL;
  }
  
  master->htbl = calloc(HASHTABLE_STARTSIZE, sizeof(*(master->htbl)));
  if(master->htbl == NULL) {
    free(master);
    return NULL;
  }

  master->size = HASHTABLE_STARTSIZE;
  master->hash = hash;
  master->rehash = rehash;
  master->cmp = cmp;
  master->dupe = dupe;
  master->undupe = undupe;
  
  /* initialise the status portion */
  master->hstatus.probes = master->hstatus.misses = 0;
  master->hstatus.hentries = 0;
  master->hstatus.hdeleted = 0;
  master->hstatus.herror = hshOK;
  
  return master;
}

void hashtable_free(hashtable *m)
{
  unsigned long i;
  void *h;
  
  if(m == NULL) {
    return;
  }
  
  /* unload the actual data storage */
  for (i = 0; i < m->size; i++) {
    if ((h = m->htbl[i]) && ((void*)m != h)) {
      if(m->undupe != NULL) {
	m->undupe(m->htbl[i]);
      }
    }
  }
  
  /* free the table */
  free(m->htbl);
  /* free the container structure */
  free(m);
}


void *hashtable_insert(hashtable *m, void *item)
{
  if(m == NULL) {
    return NULL;
  }

  if ((TSPACE(m) <= 0) && !reorganize(m)) {
    m->hstatus.herror |= hshTBLFULL;
    return NULL;
  }

  return putintbl(m, item, 0);
}


void *hashtable_find(hashtable *m, void *item)
{
  unsigned long h;

  if(m == NULL) {
    return NULL;
  }

  h = huntup(m, item);
  return m->htbl[h];
}


void *hashtable_remove(hashtable *m, void *item)
{
  unsigned long h;
  void *olditem;

  if(m == NULL) {
    return NULL;
  }

  h = huntup(m, item);
  olditem = m->htbl[h];

  if(olditem != NULL) {
    /* todo: why arent we setting this to NULL? */
    m->htbl[h] = (void*)m;
    m->hstatus.hdeleted++;
  }
  
  return olditem;
}


int hashtable_foreach(hashtable *m, hshexecfn exec, void *datum)
{
  int err;
  unsigned long i;
  void *hh;

  if ((m == NULL) ||
      (exec == NULL)) {
    return -1; 
  }

  for (i = 0; i < m->size; i++) {
    hh = m->htbl[i];
    if((hh != NULL) &&
       (hh != (void*)m)) {
      err = exec(hh, datum);
      if(err != 0) {
	return err;
      }
    }
  }

  return 0;
}

hshstats *hashtable_stats(hashtable *m)
{
  if(m == NULL) {
    return NULL;
  }
  
  return &(m->hstatus);
}


/* ============= Useful generic functions ============= */

/* NOTE: hash is called once per operation, while rehash is
   called _no more_ than once per operation.  Thus it
   is preferable that hash be the more efficient.
*/

unsigned long hshstrhash(const char * string)
{
  /* This is an implementation of sdbm string hashing */  
  unsigned long hash = 0;
  while(*string) {
    hash = *string++ + ( hash << 6 ) + ( hash << 16 ) - hash; 
  }
  return hash;
}

unsigned long hshstrehash(const char * string)
{
  /* this is an implementation of rs string hashing */
  unsigned long hash = 0;
  unsigned long a = 63689;
  unsigned long b = 378551;

  while(*string) {
    hash = hash*a + *string++;
    a *= b;
  }  
 
  return hash;
}

/* So the prime of interest, vs index i into above table,   */
/* is    ( 2**(FIRSTN + i) ) - primetbl[i]                  */
/* The above table suffices for about 48,000,000 entries.   */

/* return a prime slightly less than 2**(FIRSTN + i) -1 */
/* return 0 for i value out of range                    */
static unsigned long ithprime(size_t i)
{
  if ((i < sizeof primetbl / sizeof (int)) && (primetbl[i])) {
    return ((1 << (FIRSTN + i)) - primetbl[i]);
  } else {
    return 0;
  }
}


/* Attempt to insert item at the hth position in the table */
/* Returns NULL if position already taken or if dupe fails */
/* (when master->herror is set to hshNOMEM)                */
static void *inserted(hashtable *master, 
		      unsigned long h,
		      void *item,
		      int copying)  /* during reorganization */
{
  void  *hh;

  /* increment probe counter */
  master->hstatus.probes++;

  hh = master->htbl[h];

  if (hh == NULL) {
    /* slot is empty */
    /* copying is a misnomer here, what it really means is, "am I
       making a copy of the old table (and thus do not need to dup,
       its already been dup'd once, or, am I not making a copy-table,
       and should attempty to make a dup of the item */
    if(copying) {  
      master->htbl[h] = item;
    } else if ((master->htbl[h] = (master->dupe == NULL) ? item : master->dupe(item))) {
      /* new entry, so dupe and insert */
      master->hstatus.hentries++;          /* count 'em */
    } else {
      master->hstatus.herror |= hshNOMEM;
    }
  } else if (copying) {
    return NULL; /* no compare if copying */
  } else if (hh == (void*)master) {
    return NULL;  /* nor if DELETED */
  } else if (0 != master->cmp(master->htbl[h], item)) {
    /* not found here */
    return NULL;
  }

  /* else found already inserted here */
  return master->htbl[h];
} /* inserted */

static void *putintbl(hashtable *master, void *item, int copying)
{
  unsigned long h;
  unsigned long h2;
  void *stored;

  h = master->hash(item) % master->size;
  stored = inserted(master, h, item, copying);

  if ((stored == NULL) && 
      (master->hstatus.herror == hshOK)) {
    /* if the item was not already in the table, and we do not have
       any errors */
    h2 = master->rehash(item) % (master->size >> 3) + 1;
    do {       /* we had to go past 1 per item */
      master->hstatus.misses++;
      h = (h + h2) % master->size;

      stored = inserted(master, h, item, copying);
    } while ((stored == NULL) &&
	     (master->hstatus.herror == hshOK));
  }
  return stored;
}

/* Increase the table size by roughly a factor of 2    */
/* reinsert all entries from the old table in the new. */
/* revise the size value to match                 */
/* free the storage for the old table.                 */
static int reorganize(hashtable *master)
{
  void **newtbl;
  void **oldtbl;
  unsigned long newsize, oldsize;
  unsigned long oldentries, j;
  unsigned int i;

  oldsize = master->size;
  oldtbl =  master->htbl;
  oldentries = 0;

  if (master->hstatus.hdeleted > (master->hstatus.hentries / 4))
    /* don't expand table if we can get reasonable space by simply
       removing the accumulated DELETED entries - reasonable space
       being that more than 1/4 of the total entries have been
       deleted */
    newsize = oldsize;
  else {
    /* all ithprime usage is here */
    newsize = ithprime(0);
    for (i = 1; newsize && (newsize <= oldsize); i++) {
      newsize = ithprime(i);
    }
  }

  if (newsize != 0) {
    newtbl = calloc(newsize, sizeof(*newtbl));
  } else {
    /* this is an error being returned - even though its not -really-
       a bad thing */
    return 0;
  }

  master->size = newsize;
  master->htbl = newtbl;
  
  /* Now reinsert all old entries in new table */
  for (j = 0; j < oldsize; j++) {
    if ((oldtbl[j] != NULL) &&
	(oldtbl[j] != (void*)master)) {
      (void) putintbl(master, oldtbl[j], 1);
      oldentries++;
    }
  }

  /* Sanity check */
  if (oldentries != 
      (master->hstatus.hentries - master->hstatus.hdeleted)) {
    /* set the hashtable status to an error */
    master->hstatus.herror |= hshINTERR;

    /* free the new table */
    free(master->htbl);

    /* restore the old table */
    master->htbl = oldtbl;
    master->size = oldsize;
    return 0;
  } else {
   
    /* set the new statistics */
    master->hstatus.hentries = oldentries;
    master->hstatus.hdeleted = 0;

    /* free the old table */
    free(oldtbl);
  }

  return 1;
}

/* Attempt to find item at the hth position in the table */
/* counting attempts.  Returns 1 if found, else 0        */
static int found(hashtable *master, unsigned long h, void *item)
{
  void *hh;

  /* increment total probecounter */
  master->hstatus.probes++;
  
  hh = master->htbl[h];
  
  if((hh == NULL) ||
     (hh == (void*)master)) {
    return 0;
  }
  
  return !(master->cmp(hh, item));
}

/* Find the current hashtbl index for item, or an empty slot */
static unsigned long huntup(hashtable *master, void *item)
{
  unsigned long h;
  unsigned long h2;

  /* limit h to the size of the table */
  h = master->hash(item) % master->size;

  /* Within this a DELETED item simply causes a rehash */
  /* i.e. treat it like a non-equal item               */

  if (!(found(master, h, item)) && master->htbl[h]) {
    h2 = master->rehash(item) % (master->size >> 3) + 1;
    do {       /* we had to go past 1 per item */
      master->hstatus.misses++;
      h = (h + h2) % master->size;
    } while (!(found(master, h, item)) && (master->htbl[h]));
  }

  return h;
}

