#include "kacchios.h"

// ============================================================
// SIMPLE PROCESS SYNCHRONIZATION EXAMPLE
// Main waits for worker to complete using a semaphore
// ============================================================

int done_sem;  // Semaphore for synchronization

void worker_process(void* arg) {
    (void)arg;
    
    serial_puts("[Worker] Starting work...\n");
    
    // Simulate some work with busy wait
    for(volatile long i=0; i<100000000; i++);
    
    serial_puts("[Worker] Work completed! Signaling main...\n");
    
    // Signal main that we're done
    sem_signal(done_sem);
}

void main(void) {
    serial_puts("\n\n");
    serial_puts("===========================================\n");
    serial_puts("  SIMPLE SYNCHRONIZATION EXAMPLE\n");
    serial_puts("  (Main waits for worker to complete)\n");
    serial_puts("===========================================\n\n");
    
    // Create semaphore with count=0 (main will block on it)
    done_sem = sem_create(0);
    
    if(done_sem < 0) {
        serial_puts("[ERROR] Failed to create semaphore!\n");
        return;
    }
    
    serial_puts("[Main] Creating worker process...\n");
    create_process(worker_process, NULL, "worker");
    
    serial_puts("[Main] Waiting for worker to complete...\n\n");
    
    // Wait for worker to signal completion
    sem_wait(done_sem);
    
    // This prints AFTER worker completes
    serial_puts("\n[Main] Worker finished! Main continuing...\n");
    serial_puts("[Main] This message appears AFTER worker.\n");
    serial_puts("===========================================\n\n");
    
    // Cleanup
    sem_delete(done_sem);
}
