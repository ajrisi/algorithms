/**
 * @file   cmp.h
 * @author Adam Risi <ajrisi@gmail.com>
 * @date   Tue Oct 20 16:16:26 2009
 * 
 * @brief Comparator Routines, useful for when you need to compare
 * things
 * 
 * 
 */

#ifndef _CMP_H_
#define _CMP_H_

#include <stdlib.h>
#include <string.h>

typedef int(*cmpfn)(void*,void*);

int cmpchr(char *x, char *y);

int fsubstr(char *haystack, char *needle);

#endif
