/*
 * malloc_simple.c
 *   A simple sequence of allocations and frees.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CHUNK 1

main()
{
  void *a, *b, *c, *d, *e;

  a = malloc(2*CHUNK);
  printf("  a     = %p\n",a);
  b = malloc(6*CHUNK);
  printf("  b     = %p\n",b);
  c = malloc(1*CHUNK);
  printf("  c     = %p\n",c);

  free(a);

  d = malloc(3*CHUNK);
  printf("  d     = %p\n",d);

  free(b);

  e = malloc(3*CHUNK);
  printf("  e     = %p\n",e);
}
