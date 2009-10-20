#include "cmp.h"

int cmpchr(char *x, char *y)
{
  return *y - *x;
}


int fsubstr(char *haystack, char *needle)
{
  int i;
  unsigned int delta1[256];
  unsigned int needle_len;

  needle_len = strlen(needle);

  for(i = 0; i < 254; i++) {
    delta1[i] = needle_len + 1;
  }

  for(i = 0; i < needle_len; ++i) {
    delta1[(int)needle[i]] = needle_len - 1;
  }
  

  char *tx = haystack;
  char *px = needle;
  
  unsigned int plen = strlen(needle);
  
  if(strlen(haystack) < plen) {
    return 0;
  }
  
  char *end = tx + strlen(haystack) - plen + 1;
  while(tx < end) {
    unsigned int i = 0;
    for(; px[i] && (px[i] == tx[i]); ++i)
      /* empty */
      ;
    
    if(px[i] == 0)
      return 1;
    
    tx += delta1[(int)tx[plen]];
  }
  
  return 0;
  
}

