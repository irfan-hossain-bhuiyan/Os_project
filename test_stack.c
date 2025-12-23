#include "stack.h"
#include <stdio.h>

int main() {
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
