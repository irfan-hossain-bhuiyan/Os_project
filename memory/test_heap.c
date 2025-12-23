#include "heap.h"
#include <stdio.h>

int main() {
    void* blocks[5];
    int i;
    // Allocate 5 blocks of 1024 bytes
    for (i = 0; i < 5; ++i) {
        blocks[i] = alloc_heap(1024);
        if (!blocks[i]) {
            printf("Failed to allocate heap block %d\n", i);
            return 1;
        }
        printf("Allocated heap block %d at %p\n", i, blocks[i]);
    }
    // Try to free a block (should fail)
    if (free_heap(blocks[0]) == 0) {
        printf("Error: individual free succeeded!\n");
        return 2;
    }
    // Reset the heap
    if (free_heap(blocks[0]) != 0 && free_heap((void*)0) != 0) {
        printf("Heap reset failed (expected only heap base can reset)\n");
        return 3;
    }
    if (free_heap((void*)(&blocks[0])) == 0) {
        printf("Error: non-base free succeeded!\n");
        return 4;
    }
    // Reset heap using base pointer
    if (free_heap(heap_base()) == 0) {
        printf("Heap reset succeeded.\n");
    } else {
        printf("Heap reset failed.\n");
        return 5;
    }
    return 0;
}

