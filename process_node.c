#include "process_node.h"

process_node* next_node(uint8_allocator *alloc, process_node *node) {
    if (!node || node->next == 255) return NULL;
    return &alloc->array[node->next];
}

process_node* prev_node(uint8_allocator *alloc, process_node *node) {
    if (!node || node->prev == 255) return NULL;
    return &alloc->array[node->prev];
}
