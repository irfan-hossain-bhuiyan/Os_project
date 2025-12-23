#include "heap.h"
#include "stack.h"
#include <stdio.h>

int test_heap() {
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

int test_stack() {
    void* stacks[5];
    int i;
    // Allocate 5 stacks
    for (i = 0; i < 5; ++i) {
        stacks[i] = alloc_stack(4096);
        if (!stacks[i]) {
            printf("Failed to allocate stack %d\n", i);
            return 1;
        }
        printf("Allocated stack %d at %p\n", i, stacks[i]);
    }
    // Free all stacks
    for (i = 0; i < 5; ++i) {
        if (free_stack(stacks[i]) != 0) {
            printf("Failed to free stack %d\n", i);
            return 2;
        }
        printf("Freed stack %d\n", i);
    }
    // Try to free again (should fail)
    if (free_stack(stacks[0]) == 0) {
        printf("Error: double free succeeded!\n");
        return 3;
    }
    printf("Double free correctly failed.\n");
    return 0;
}

int main() {
    int result = 0;
    printf("Running heap tests...\n");
    result = test_heap();
    if (result != 0) {
        printf("Heap tests failed with code %d\n", result);
        return result;
    }
    printf("Heap tests passed.\n\n");
    printf("Running stack tests...\n");
    result = test_stack();
    if (result != 0) {
        printf("Stack tests failed with code %d\n", result);
        return result;
    }
    printf("Stack tests passed.\n");
    return 0;
}
