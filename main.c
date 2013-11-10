#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "malloc.h"

#define NUM_ARRAY 500
#define align8(x) (((((x)-1) >> 3) << 3) + 8)
int main(void) {
    printf("---- INIT ----\n\n");
    int** arrays = (int**) malloc(NUM_ARRAY * sizeof(int*));
    for (int i = 1; i <= NUM_ARRAY; i++) {
        printf("---- Run %i ----\n", i);
        arrays[i-1] = (int*) malloc(i * sizeof(int));
        if (arrays[i-1] == NULL) {
            perror("malloc: ");
            printf("OOM: Dying\n");
            exit(errno);
        }
        printf("Size: %ld Loc: %p\n", align8(32 + i * sizeof(int)), arrays[i-1]);
        for (int j = 0; j < i; j++) {
            arrays[i-1][j] = 1 + j;
        }
    }

    printf("\nChecking for corruption...\n");
    for (int m = 0; m < NUM_ARRAY; m++) {
        for (int n = 0; n < m; n++) {
            if (arrays[m][n] != 1 + n) {
                printf("Corruption detected at %i:%i\n", m, n);
                printf("Expected %i; Found %i\n", 1 + n, arrays[m][n]);
            }
        }
    }

    printf("\n---- FREE ARRAYS ----\n");
    for (int k = 0; k < NUM_ARRAY; k++) {
        free(arrays[k]);
    } 
    printf("Arrays freed, done\n");
}
