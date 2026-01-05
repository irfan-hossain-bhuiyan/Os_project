#include "heap.h"
#include "debug.h"

#define HEAP_SIZE (64 * 1024) // 64KB Heap
#define ALIGNMENT 16

// Header for each memory block
// We need to ensure the header itself is aligned or handled such that the data following it is aligned.
// If ALIGNMENT is 16, and we return (address + sizeof(BlockHeader)), sizeof(BlockHeader) should be a multiple of 16
// OR we handle the offset calculation carefully.
// Simplest way: Make struct BlockHeader size a multiple of 16.

struct BlockHeader
{
    size_t size;              // Size of the data block (excluding header)
    struct BlockHeader *next; // Next block in the list
    uint8_t is_free;          // 1 if free, 0 if used
    uint8_t padding[7];       // Padding to ensure sizeof(BlockHeader) is 16 bytes (4+4+1+7=16) on 32-bit
};

// The global heap memory array
static uint8_t heap_memory[HEAP_SIZE] __attribute__((aligned(ALIGNMENT)));

// Head of the linked list
static struct BlockHeader *heap_head = NULL;

void heap_init(void)
{
    if (heap_head != NULL)
        return; // Already initialized

    heap_head = (struct BlockHeader *)heap_memory;
    // The first block covers the whole heap minus the first header
    heap_head->size = HEAP_SIZE - sizeof(struct BlockHeader);
    heap_head->next = NULL;
    heap_head->is_free = 1;

    // kdebug_puts("[HEAP] Initialized 64KB heap with 16-byte alignment.\n");
}

// Malloc: Allocates memory
void *malloc(size_t size)
{
    // Note: heap_init() must be called once during kernel startup!
    if (heap_head == NULL)
    {
        // Auto-init fallback
        heap_init();
    }

    if (size == 0)
        return NULL;

    // Align the requested size
    size_t aligned_size = (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);

    struct BlockHeader *current = heap_head;
    while (current != NULL)
    {
        if (current->is_free && current->size >= aligned_size)
        {
            // Found a suitable block

            // Can we split it? We need enough space for a new header + at least ALIGNMENT bytes
            if (current->size >= aligned_size + sizeof(struct BlockHeader) + ALIGNMENT)
            {
                struct BlockHeader *new_block = (struct BlockHeader *)((uint8_t *)current + sizeof(struct BlockHeader) + aligned_size);

                new_block->size = current->size - aligned_size - sizeof(struct BlockHeader);
                new_block->next = current->next;
                new_block->is_free = 1;

                current->size = aligned_size;
                current->next = new_block;
            }

            current->is_free = 0;
            // Return pointer to the data area (just after the header)
            return (void *)((uint8_t *)current + sizeof(struct BlockHeader));
        }
        current = current->next;
    }

    // klog_error("malloc: Out of memory");
    return NULL;
}

// Free: Frees memory
void free(void *ptr)
{
    if (ptr == NULL)
        return;

    // Get the header (it sits immediately before the pointer)
    struct BlockHeader *header = (struct BlockHeader *)((uint8_t *)ptr - sizeof(struct BlockHeader));

    // Sanity check: ensure this pointer is actually within our heap range
    if ((uint8_t *)header < heap_memory || (uint8_t *)header >= heap_memory + HEAP_SIZE)
    {
        // klog_error("free: Invalid pointer (outside heap range)");
        return;
    }

    header->is_free = 1;

    // Coalesce (Merge) with the next block if it's free
    if (header->next != NULL && header->next->is_free)
    {
        header->size += sizeof(struct BlockHeader) + header->next->size;
        header->next = header->next->next;
    }

    // Forward Coalescing (Full Sweep)
    struct BlockHeader *current = heap_head;
    while (current != NULL)
    {
        if (current->is_free && current->next != NULL && current->next->is_free)
        {
            current->size += sizeof(struct BlockHeader) + current->next->size;
            current->next = current->next->next;
        }
        else
        {
            current = current->next;
        }
    }
}

// Realloc: Resizes memory
void *realloc(void *ptr, size_t size)
{
    if (ptr == NULL)
        return malloc(size);
    if (size == 0)
    {
        free(ptr);
        return NULL;
    }

    // Get the header
    struct BlockHeader *header = (struct BlockHeader *)((uint8_t *)ptr - sizeof(struct BlockHeader));

    // Align the requested size
    size_t aligned_size = (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);

    // Optimization 1: If current block is already large enough, just return it
    if (header->size >= aligned_size)
    {
        return ptr;
    }

    // Optimization 2: Check if next block is free and if merging would provide enough space
    if (header->next != NULL && header->next->is_free)
    {
        size_t total_space = header->size + sizeof(struct BlockHeader) + header->next->size;
        if (total_space >= aligned_size)
        {
            // Absorb the next block
            header->size = total_space;
            header->next = header->next->next;
            // Now we fit!
            return ptr;
        }
    }

    // Fallback: Allocate new block, copy data, free old block
    void *new_ptr = malloc(size);
    if (new_ptr)
    {
        uint8_t *src = (uint8_t *)ptr;
        uint8_t *dst = (uint8_t *)new_ptr;
        size_t copy_size = header->size;
        for (size_t i = 0; i < copy_size; i++)
        {
            dst[i] = src[i];
        }
        free(ptr);
    }
    return new_ptr;
}

// Get heap statistics
void heap_stats(size_t *used, size_t *free_space, size_t *total)
{
    if (heap_head == NULL)
    {
        *used = 0;
        *free_space = 0;
        *total = 0;
        return;
    }

    *total = HEAP_SIZE;
    *used = 0;
    *free_space = 0;

    struct BlockHeader *current = heap_head;
    while (current != NULL)
    {
        size_t block_total = sizeof(struct BlockHeader) + current->size;
        if (current->is_free)
        {
            *free_space += current->size;
        }
        else
        {
            *used += current->size;
        }
        current = current->next;
    }
}
