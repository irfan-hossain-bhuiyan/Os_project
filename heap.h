#ifndef MEMORY_HEAP_H
#define MEMORY_HEAP_H

#include "types.h"

void heap_init(void);

// Standard-like wrappers
void* malloc(size_t size);
void free(void* ptr);
void* realloc(void* ptr, size_t size);

// Returns the base pointer of the heap.
void* heap_base(void);

#endif // MEMORY_HEAP_H
