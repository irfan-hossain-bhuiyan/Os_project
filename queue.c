#include "queue.h"

// Macro to implement a queue for any type
#define DEFINE_QUEUE_TYPE(TYPENAME, TYPE, SIZE) \
void TYPENAME##_enqueue(TYPENAME##_queue *q, TYPE val) { \
    if (((q->tail + 1) % SIZE) == q->head) return; /* Full */ \
    q->data[q->tail] = val; \
    q->tail = (q->tail + 1) % SIZE; \
} \
TYPE TYPENAME##_dequeue(TYPENAME##_queue *q) { \
    if (q->head == q->tail) return (TYPE)0; /* Empty */ \
    TYPE val = q->data[q->head]; \
    q->head = (q->head + 1) % SIZE; \
    return val; \
} \
int TYPENAME##_empty(TYPENAME##_queue *q) { \
    return q->head == q->tail; \
} \
int TYPENAME##_full(TYPENAME##_queue *q) { \
    return ((q->tail + 1) % SIZE) == q->head; \
}

// Implement the uint8_t queue
DEFINE_QUEUE_TYPE(uint8, uint8_t, 256)
