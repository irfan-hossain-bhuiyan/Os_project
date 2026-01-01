#include <stdio.h>

// Fix NULL redefinition conflict
#undef NULL
#include "heap.h"



int main() {
    printf("--- Heap Allocator Test ---\n");

    // 1. Initialize Heap
    printf("[1] Initializing Heap...\n");
    heap_init();

    // 2. Simple Allocation
    printf("[2] Testing Basic Allocation...\n");
    void* ptr1 = malloc(1024); // 1KB
    if (!ptr1) { printf("FAILED: ptr1 is NULL\n"); return 1; }
    printf("    ptr1 allocated at %p\n", ptr1);

    void* ptr2 = malloc(256); // 256 bytes
    if (!ptr2) { printf("FAILED: ptr2 is NULL\n"); return 1; }
    printf("    ptr2 allocated at %p\n", ptr2);

    // 3. Write/Read Test
    printf("[3] Testing Write/Read...\n");
    
    // Manual memset (0xAA)
    unsigned char* p1 = (unsigned char*)ptr1;
    for(int i=0; i<1024; i++) p1[i] = 0xAA;

    // Manual memset (0xBB)
    unsigned char* p2 = (unsigned char*)ptr2;
    for(int i=0; i<256; i++) p2[i] = 0xBB;
    
    // Verify first byte
    if (*(unsigned char*)ptr1 != 0xAA) { printf("FAILED: ptr1 data mismatch\n"); return 1; }
    if (*(unsigned char*)ptr2 != 0xBB) { printf("FAILED: ptr2 data mismatch\n"); return 1; }
    printf("    Data integrity verified.\n");

    // 4. Free and Reuse
    printf("[4] Testing Free and Reuse...\n");
    free(ptr1); // Free the first 1KB block
    
    // This new allocation should fit into the hole left by ptr1
    void* ptr3 = malloc(512); 
    printf("    ptr3 allocated at %p\n", ptr3);

    if (ptr3 == ptr1) {
        printf("    SUCCESS: ptr3 reused ptr1's address (Memory Reused)\n");
    } else {
        printf("    WARNING: ptr3 did not reuse ptr1 (Fragmentation or Logic variance)\n");
    }

    // 5. Coalescing Test
    printf("[5] Testing Coalescing...\n");
    free(ptr3);
    free(ptr2); 
    // Now both 1KB (ptr1/ptr3 slot) and 256B (ptr2 slot) are free.
    // They should be merged. A large allocation should succeed.
    
    void* ptr4 = malloc(2048); // 2KB
    printf("    ptr4 allocated at %p\n", ptr4);
    if (!ptr4) { printf("FAILED: Coalescing failed, heap too fragmented?\n"); return 1; }

    printf("--- Heap Test Passed ---\n");
    return 0;
}
