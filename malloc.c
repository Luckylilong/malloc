#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "malloc.h"

// performs value alignment inline
#define align8(x) (((((x)-1) >> 3) << 3) + 8)
// avoid dependency on math.h for one function
#define ceil_loc(x) (((x) - (int) (x)) > 0 ? (int) (x) + 1 : (int) (x))

static list_node* head = NULL;

// locates size number of bytes on the heap, rounded up to the nearest multiple of 8
void *malloc(size_t size) {
    long pg_size = sysconf(_SC_PAGESIZE);
    int stored = 0;
    list_node* current;
    if (head == NULL) {
        // set up head node, bump break appropriate size for request
        int block_size = align8(size + sizeof(list_node));
        int num_pg = ceil_loc((float) block_size / pg_size);
        head = (list_node*) sbrk(pg_size * num_pg);
        if (head == (void*) - 1) {
            return NULL;
        }
        // clear pages requested 
        /*memset(head, 0, pg_size * num_pg); */
        // set size allocted to block + header
        head->size = align8(sizeof(list_node) + size); 
        head->next = NULL;
        head->free = false;
        return (void*) head + sizeof(list_node);
    }

    // set up current and traverse linked list
    current = head;
    // if has next node, and node is not free enough, traverse list
    while (current->next != NULL) {
        // Current node is free and right size, mark used and return
        if (current->free && (current->size - sizeof(list_node)) >= size) {
            current->free = false;
            // clear to expected value
            /*memset((void*) current + sizeof(list_node), 0, current->size - sizeof(list_node));*/
            return ((void*) current + sizeof(list_node));
        }
        stored += current->size;
        current = current->next;
    }
    stored += current->size;
    long size_req = stored + (long) head + align8(sizeof(list_node) + size);
    // not enough space, bump break
    long end = (long) sbrk(0);
    if (size_req > end) {
        // figure out how many pages we need and bump
        int block_size = align8(size_req - end);
        int num_pg = ceil_loc((float) block_size / pg_size);
        end = (long) sbrk(pg_size * num_pg);
        // check OOM
        if (end == -1) {
            return NULL;
        }
    }
    current->next = (list_node*) (current->size + (void*) current);
    /*memset(current->next, 0, align8(sizeof(list_node) + size));*/
    // move current to new end of list
    current = current->next;
    current->next = NULL; // end of list marker
    current->free = false;
    current->size = align8(sizeof(list_node) + size);
    return ((void*) current + sizeof(list_node));
}

// Frees previously malloc'd location in memory for later use
void free(void *ptr) {
    if (ptr) {
        // find header for given pointer
        list_node* current = (list_node*) ptr - 1;
        current->free = true;
    }
}

// Allocate an array, zero filled.
void *calloc(size_t nmemb, size_t size) {
    // sizing doesn't make sense, exit
    if (nmemb <= 0 || size <= 0) {
        return NULL;
    }

    int array_size = nmemb * size;
    void* ptr = malloc(array_size);
    if (!ptr) {
        return NULL;
    }
    memset(ptr, 0, array_size);
    return ptr;
}

// make requested memory location bigger, without changing contents
void *realloc(void *pointer, size_t size) {
    if (pointer) {
        // resize to 0 = free
        // resize to negative: user incompetent, destroy their memory to spite them
        if (size <= 0) {
            free(pointer);
            return NULL;
        }
        // find header, current size
        list_node* node = (list_node*) pointer - 1;
        int cur_size = node->size - sizeof(list_node);
        // resize to same size = return the array
        if (cur_size == size) {
            return pointer;
        }
        // copy data to new array, free old, exit
        void* new_ptr = malloc(size);
        // OOM
        if (!new_ptr) {
            return NULL;
        }
        memcpy(new_ptr, pointer, cur_size);
        free(pointer);
        return new_ptr;
    }
    // passed in null, get null back.
    return NULL;
}
