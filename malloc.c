#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "malloc.h"

// performs next value alignment inline
// x is wrapped in its own paren to evaluate expressions passed in
// before rounding operation
#define align8(x) (((((x)-1) >> 3) << 3) + 8)

static list_node* head = NULL;

void *malloc(size_t size) {
    long pg_size = sysconf(_SC_PAGESIZE);
    int stored = 0;
    long end = (long) sbrk(0);
    list_node* current;
    if (head == NULL) {
        // set up head node
        head = (list_node*) sbrk(pg_size);
        end = (long) head;
        // check OOM condition
        if (end == -1) {
            errno = ENOMEM;
            return NULL;
        }
        memset(head, 0, pg_size); 
        head->next = NULL;
        head->prev = NULL;
        head->size = align8(sizeof(list_node) + size); 
        return head + size;
    }

    // set up current and traverse linked list
    current = head;
    while (current->next != NULL) {
        stored += current->size;
        current = current->next;
    }
    if (align8(stored + (long) head + size) >= end) {
        end = (long) sbrk(pg_size);
        memset(, 0, pg_size); 
        // check OOM
        if (end == -1) {
            errno = ENOMEM;
            return NULL;
        }
    }
    current->next = (list_node*) align8(current->size + size);
    // assign previous pointer of next node to current node
    current->next->prev = current;
    current = current->next;
    current->size = align8(sizeof(list_node) + size);
}
