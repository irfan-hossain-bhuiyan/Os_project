#include "allocator.h"
#include "serial.h"

// Macro to implement a generic bitmap-based allocator for any type
#define DEFINE_ALLOCATOR_TYPE(TYPENAME, TYPE, SIZE) \
int TYPENAME##_alloc(TYPENAME##_allocator *a) { \
    int idx = bitmap_first_zero(&a->used); \
    if (idx == -1) { \
        serial_puts("[ALLOC ERROR] Out of slots!\n"); \
        return -1; \
    } \
    bitmap_set(&a->used, (uint8_t)idx); \
    serial_puts("[ALLOC] Allocated index: "); \
    serial_print_hex(idx); \
    serial_puts("\n"); \
    return idx; \
} \
int TYPENAME##_dealloc(TYPENAME##_allocator *a, uint8_t idx) { \
    if (idx >= SIZE) { \
        serial_puts("[DEALLOC ERROR] Invalid index!\n"); \
        return -1; \
    } \
    if (!bitmap_get(&a->used, idx)) { \
        serial_puts("[DEALLOC ERROR] Index not allocated!\n"); \
        return -1; \
    } \
    bitmap_clear(&a->used, idx); \
    serial_puts("[DEALLOC] Freed index: "); \
    serial_print_hex(idx); \
    serial_puts("\n"); \
    return 0; \
}

// Implement the uint8_t allocator
DEFINE_ALLOCATOR_TYPE(uint8, uint8_t, ALLOCATOR_SIZE)
