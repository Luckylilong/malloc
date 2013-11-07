#include <stdio.h>
#include <unistd.h>
#include "malloc.h"

static list_node head = { 0, 0 };

void *malloc(size_t size) {
    if (head.next == 0) {
        head.next = (list_node*) sbrk(0);
    }

    printf("%i", head.next);
}
