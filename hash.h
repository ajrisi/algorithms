#ifndef _HASH_H_
#define _HASH_H_

/**
 * In this file, if we define NETHASH, we get functions to generate a hash from a sockaddr
 * 
 */
#ifdef NETHASH
#include <netinet/in.h>
#endif

unsigned long hash_string(const char *string);

unsigned long rehash_string(const char *string);

unsigned long long llhash_general(void *data, unsigned int length);




#ifdef NETHASH

#define SOCKADDR_SIZEOF(x) (x->sa_family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6))

unsigned long hash_sockaddr(struct sockaddr *sa);

unsigned long rehash_sockaddr(struct sockaddr *sa);

#endif




#endif
