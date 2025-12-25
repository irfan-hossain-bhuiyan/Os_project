// Generic macro to declare a queue type and its functions for any data type.
#define DECLARE_QUEUE_TYPE(TYPENAME, TYPE, SIZE) \
typedef struct { \
    TYPE data[SIZE]; \
    int head, tail; \
} TYPENAME##_queue; \
void TYPENAME##_enqueue(TYPENAME##_queue *q, TYPE val); \
TYPE TYPENAME##_dequeue(TYPENAME##_queue *q); \
int TYPENAME##_empty(TYPENAME##_queue *q); \
int TYPENAME##_full(TYPENAME##_queue *q);

// Declare a uint8_t queue with max 256 elements
#include <stdint.h>
DECLARE_QUEUE_TYPE(uint8, uint8_t, 256)
