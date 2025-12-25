#ifndef MEMORY_HEAP_H
#define MEMORY_HEAP_H

#include "types.h"
// Allocates a block from the heap. Returns pointer or NULL on error.
void* alloc_heap(size_t size);
// Frees a previously allocated heap block. Returns 0 on success, negative on error.
int free_heap(void* ptr);
// Returns the base pointer of the heap.
void* heap_base(void);


#endif // MEMORY_HEAP_H
