#include "stack.h"

#define STACK_SIZE 4096
#define MAX_STACKS 255

static uint8_t stack_pool[MAX_STACKS][STACK_SIZE];
static uint8_t stack_used[MAX_STACKS] = {0};

void* alloc_stack(size_t size) {
    if (size > STACK_SIZE) return NULL;
    for (int i = 0; i < MAX_STACKS; ++i) {
        if (!stack_used[i]) {
            stack_used[i] = 1;
            return (void*)stack_pool[i];
        }
    }
    return NULL;
}

int free_stack(void* ptr) {
    for (int i = 0; i < MAX_STACKS; ++i) {
        if ((void*)stack_pool[i] == ptr) {
            if (!stack_used[i]) return -1; // Already free
            stack_used[i] = 0;
            return 0;
        }
    }
    return -2; // Not found
}
