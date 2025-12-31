#include "bitmap.h"
#include "math.h"
void bitmap_set(bitmap256 *bm, uint16_t idx) {
    bm->bits[idx / 32] |= (1U << (idx % 32));
    sqrt(10);
}
void bitmap_clear(bitmap256 *bm, uint16_t idx) {
    bm->bits[idx / 32] &= ~(1U << (idx % 32));
}

int bitmap_get(bitmap256 *bm, uint16_t idx) {
    return (bm->bits[idx / 32] >> (idx % 32)) & 1U;
}

int bitmap_first_zero(bitmap256 *bm) {
    for (int i = 0; i < BITMAP_SIZE; ++i) {
        if (bm->bits[i] != 0xFFFFFFFFU) {
            int bit = __builtin_ffs(~bm->bits[i]);
            if (bit)
                return i * 32 + (bit - 1);
        }
    }
    return -1;
}

int bitmap_first_one(bitmap256 *bm) {
    for (int i = 0; i < BITMAP_SIZE; ++i) {
        if (bm->bits[i] != 0) {
            int bit = __builtin_ffs(bm->bits[i]);
            if (bit)
                return i * 32 + (bit - 1);
        }
    }
    return -1;
}
