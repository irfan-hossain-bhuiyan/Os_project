#include "heap.h"

#define HEAP_SIZE (64 * 1024) // 64KB heap
#define ALIGNMENT 8

static uint8_t heap[HEAP_SIZE];
static size_t heap_offset = 0;

void* alloc_heap(size_t size) {
    // Align size
    size = (size + (ALIGNMENT-1)) & ~(ALIGNMENT-1);
    if (heap_offset + size > HEAP_SIZE) return NULL;
    void* ptr = &heap[heap_offset];
    heap_offset += size;
    return ptr;
}

int free_heap(void* ptr) {
    // Simple bump allocator: cannot free individual blocks
    // Only allow freeing the entire heap (reset)
    if (ptr == heap) {
        heap_offset = 0;
        return 0;
    }
    return -1;
}
