#pragma once
#include <stdint.h>

#define BITMAP_SIZE 8 // 8 * 32 = 256 bits

typedef struct {
    uint32_t bits[BITMAP_SIZE];
} bitmap256;

// Set the bit at index (0..255)
void bitmap_set(bitmap256 *bm, uint16_t idx);
// Clear the bit at index (0..255)
void bitmap_clear(bitmap256 *bm, uint16_t idx);
// Get the bit at index (0..255), returns 0 or 1
int bitmap_get(bitmap256 *bm, uint16_t idx);
// Find the first zero bit, returns bit index or -1 if full
int bitmap_first_zero(bitmap256 *bm);
// Find the first one bit, returns bit index or -1 if none
int bitmap_first_one(bitmap256 *bm);
