/**
 * @file   prng.h
 * @author Adam Risi <ajrisi@gmail.com>
 * @date   Sat Sep 26 19:34:32 2009
 * 
 * @brief This file contains the basic routines for generating random
 * numbers. Most of the functional code in this file came from
 * "Numerical Recipies in C"
 * 
 * 
 */

#ifndef PRNG_H_
#define PRNG_H_

typedef struct prng_s prng;
struct prng_s {
  unsigned long long u;
  unsigned long long v;
  unsigned long long w;
};

/** 
 * Creates a new prng in memory. Needs to be freed using prng_free
 * when you are done with it
 * 
 * @param seed the seed to initialize the prng with.
 * 
 * @return a new prng, or NULL if the memory for the generator could
 * not be allocated
 */
prng *prng_new(unsigned long long seed);

/** 
 * Get a random number from the generator, type unsigned long long
 * 
 * @param r the prng
 * 
 * @return a random unsigned long long
 */
unsigned long long prng_ll(prng *r);

/** 
 * Get a random number from the generator, type double
 * 
 * @param r the prng
 * 
 * @return a random double
 */
double prng_d(prng *r);

/** 
 * Get a random number from the generator, type unsigned int
 * 
 * @param r the prng
 * 
 * @return a random int
 */
unsigned int prng_ui(prng *r);

/** 
 * Frees the memory associated with a prng created with prng_new
 * 
 */
void prng_free(prng *r);

#endif
