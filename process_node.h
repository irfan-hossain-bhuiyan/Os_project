#pragma once
#include <stdint.h>
#include "allocator.h"

// Node for process linked list (array-backed)
typedef struct {
    uint8_t process_id;
    uint8_t next;
    uint8_t prev;
} process_node;

// Returns pointer to the next node, or NULL if none
process_node* next_node(uint8_allocator *alloc, process_node *node);
// Returns pointer to the previous node, or NULL if none
process_node* prev_node(uint8_allocator *alloc, process_node *node);
