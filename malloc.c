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
        if (head == (void*) -1) {
            errno = ENOMEM;
            return NULL;
        }
        // clear pages requested 
        memset(head, 0, pg_size * num_pg); 
        // set size allocted to block + header
        head->size = align8(sizeof(list_node) + size); 
        return (void*) head + sizeof(list_node);
    }

    // set up current and traverse linked list
    current = head;
    // if has next node, and node is not free enough, traverse list
    while (current->next != NULL) {
        // Current node is free and right size, mark used and return
        if (current->free && (current->size - sizeof(list_node)) <= size) {
            current->free = false;
            // clear to expected value
            memset((void*) current + sizeof(list_node), 0, current->size - sizeof(list_node));
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
            errno = ENOMEM;
            return NULL;
        }
    }
    current->next = (list_node*) (current->size + (void*) current);
    memset(current->next, 0, align8(sizeof(list_node) + size));
    // assign previous pointer of next node to current node
    current->next->prev = current;
    // move current to new end of list
    current = current->next;
    current->next = NULL; // end of list marker
    current->free = false;
    current->size = align8(sizeof(list_node) + size);
    return ((void*) current + sizeof(list_node));
}

// Frees previously malloc'd location in memory for later use
void free(void *ptr) {
    if (ptr != NULL) {
        list_node* current = head;
        while (((void*) current + sizeof(list_node)) != ptr) {
            current = current->next;
        }
        current->free = true;
        // merge with neighbors if free
        if (current->next != NULL) {
            if (current->next->free) {
                current->size += current->next->size;
                current->next = current->next->next;
            }
        }
        if (current->prev != NULL) {
            if (current->prev->free) {
                current->prev->size += current->size;
                current->prev->next = current->next;
            }
        }
    }
}
