#ifndef MALLOC_H
#define MALLOC_H

#include <sys/types.h>

typedef struct list_node {
    size_t size;
    struct list_node* next;
} list_node;

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t num_of_elts, size_t elt_size);
void *realloc(void *pointer, size_t size);

#endif
