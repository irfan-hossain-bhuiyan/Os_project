#include "stack.h"
#include "debug.h"

#define STACK_SIZE 4096
#define MAX_STACKS 16

static uint8_t stack_pool[MAX_STACKS][STACK_SIZE] __attribute__((aligned(16)));
static uint8_t stack_used[MAX_STACKS] = {0};

void* alloc_stack(size_t size) {
    //if (size > STACK_SIZE) return NULL;
    for (int i = 0; i < MAX_STACKS; ++i) {
        if (!stack_used[i]) {
            stack_used[i] = 1;
            return (void*)(stack_pool[i]); 
        }
    }
    klog_error("[ERROR] alloc_stack: no free stacks available\n");
    return NULL;
}

int free_stack(void* ptr) {
    for (int i = 0; i < MAX_STACKS; ++i) {
        if ((void*)stack_pool[i] == ptr) {
            if (!stack_used[i]) {
                klog_error("free_stack: stack already free\n");
                return -1; // Already free
            }
            stack_used[i] = 0;
            return 0;
        }
    }
    klog_error("[ERROR] free_stack: pointer not in stack pool\n");
    return -2; // Not found
}
