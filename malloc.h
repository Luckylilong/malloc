#ifndef MALLOC_H
#define MALLOC_H

#include <sys/types.h>

typedef struct list_node {
    struct list_node* next;
    struct list_node* prev;
    size_t size;
} list_node;

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t num_of_elts, size_t elt_size);
void *realloc(void *pointer, size_t size);

#endif
