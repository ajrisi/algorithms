#include "hash.h"

unsigned long lhash_string(const char *string)
{
  /* This is an implementation of sdbm string hashing */  
  unsigned long hash = 0;
  while(*string) {
    hash = *string++ + ( hash << 6 ) + ( hash << 16 ) - hash; 
  }
  return hash;
}

unsigned long lrehash_string(const char *string)
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

unsigned long long llhash_general(void *data, unsigned int length)
{
  static unsigned long long hash_tab[256] = {0};
  static unsigned long long h;
  char *k;
  int i;
  int j;

  /* init the hash table on the first run */
  if(!hash_tab[0]) {
    h = 0x544B2FBACAAF1684LL;
    
    for (j = 0; j < 256; j++) {
      for (i = 0; i < 31; i++) {
	h = (h >> 7) ^ h;
	h = (h << 11) ^ h;
	h = (h >> 10) ^ h;
      }
      hash_tab[j] = h;
    }

  }

  k = (char*)data;
  h=0xBB40E64DA205B064LL;
  j=0;

  while (length ? j++ < length : *k) {
    h = (h * 7664345821815920749LL) ^ hash_tab[(unsigned char)(*k)];
    k++;
  }

  return h;   
}

#ifdef NETHASH

unsigned long hash_sockaddr(struct sockaddr *sa)
{
  unsigned long long h;

  if(sa == NULL) {
    return 0;
  }

  h = llhash_general((void*)sa, SOCKADDR_SIZEOF(sa));

  return *(unsigned long*)&h;
}

unsigned long rehash_sockaddr(struct sockaddr *sa)
{
  unsigned long long h;

  if(sa == NULL) {
    return 0;
  }

  h = llhash_general((void*)sa, SOCKADDR_SIZEOF(sa));

  return * ( ((unsigned long*)&h) + 1 );
}

#endif /* NETHASH */
