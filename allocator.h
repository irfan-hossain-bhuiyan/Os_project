#pragma once
#include <stdint.h>
#include "bitmap.h"

#define ALLOCATOR_SIZE 256

// Macro to declare a generic allocator for any type
#define DECLARE_ALLOCATOR_TYPE(TYPENAME, TYPE, SIZE) \
typedef struct { \
    TYPE array[SIZE]; \
    bitmap256 used; \
} TYPENAME##_allocator; \
int TYPENAME##_alloc(TYPENAME##_allocator *a); \
int TYPENAME##_dealloc(TYPENAME##_allocator *a, uint8_t idx);

// Declare a uint8_t allocator with 256 slots
DECLARE_ALLOCATOR_TYPE(uint8, uint8_t, ALLOCATOR_SIZE)
