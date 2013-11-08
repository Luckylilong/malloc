#include <stdio.h>
#include "malloc.h"

int main(void) {
    int* value = (int*) malloc(9);
    value[0] = 15;
    printf("%i\n", value[0]);
}
