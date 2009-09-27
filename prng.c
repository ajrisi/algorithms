/**
 * @file   prng.c
 * @author Adam Risi <ajrisi@gmail.com>
 * @date   Sat Sep 26 19:34:32 2009
 * 
 * @brief This file contains the basic routines for generating random
 * numbers. Most of the functional code in this file came from
 * "Numerical Recipies in C"
 * 
 * 
 */


#include <stdlib.h>

#include "prng.h"

prng *prng_new(unsigned long long seed)
{
  /* check to make sure that the long long type is atleast 64 bits,
     else return null */
  if(sizeof(long long) < 8) {
    return NULL;
  }

  prng *r = malloc(sizeof(prng));
  if(r == NULL) {
    return NULL;
  }

  r->v = 4101842887655102017LL;
  r->w = 1LL;
  
  /* check to make sure that the seed != v, as this will cause MAJOR
     PROBLEMS (the way we fix it is to just add 1 onto seed) */
  if(seed == r->v) {
    seed++;
  }

  r->u = seed ^ r->v;
  (void)prng_ll(r);

  r->v = r-> u;
  (void)prng_ll(r);

  r->w = r->v;
  (void)prng_ll(r);

  return r;
}


unsigned long long prng_ll(prng *r)
{
  unsigned long long x;

  r->u = r->u * 2862933555777941757LL + 7046029254386353087LL;

  r->v ^= r->v >> 17;
  r->v ^= r->v << 31;
  r->v ^= r->v >> 8;

  r->w = 4294957665U*(r->w & 0xffffffff) + (r->w >> 32);

  x = r->u ^ (r->u << 21);
  x ^= x >> 35; x ^= x << 4;

  return (x + r->v) ^ r->w;
}


double prng_d(prng *r)
{
  return 5.42101086242752217E-20 * prng_ll(r);
}


unsigned int prng_ui(prng *r)
{
  return (unsigned int)prng_ll(r);
}


void prng_free(prng *r)
{
  if(r == NULL) {
    return;
  }
  free(r);
}
