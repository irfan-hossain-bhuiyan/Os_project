#ifndef MEMORY_STACK_H
#define MEMORY_STACK_H

#include "types.h"

// Allocates a stack of 4KB. Returns pointer to base, or NULL on error.
void* alloc_stack(size_t size);
// Frees a previously allocated stack. Returns 0 on success, negative on error.
int free_stack(void* ptr);

#endif // MEMORY_STACK_H
