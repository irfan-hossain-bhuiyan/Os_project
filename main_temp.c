#include "kacchios.h"

// Receiver (Consumer)
void consumer(void* arg) {
    (void)arg;
    serial_puts("Consumer: Waiting for messages...\n");
    for (int i = 0; i < 5; i++) {
        uint32_t msg = receive();
        serial_puts("Consumer: Received ");
        serial_print_hex(msg);
        serial_puts("\n");
    }
    serial_puts("Consumer: Finished.\n");
    
    // Test heap
    void* ptr = malloc(128);
    if (ptr) {
        serial_puts("Consumer: Heap alloc success\n");
        free(ptr);
    }
    
    while(1);
}

// Sender (Producer)
void producer(void* arg) {
    pidtype target = (pidtype)(uintptr_t)arg;
    serial_puts("Producer: Sending messages...\n");
    for (int i = 0; i < 5; i++) {
        // Simple delay
         for (volatile int j = 0; j < 5000000; j++);
         
        serial_puts("Producer: Sending ");
        serial_print_hex(i);
        serial_puts("\n");
        
        int res = send(target, i);
        if (res != 0) {
            serial_puts("Producer: Send failed!\n");
        }
    }
    serial_puts("Producer: Finished.\n");
    while(1);
}

// User Main
void main(void* arg) {
    (void)arg;
    serial_puts("\n[User] Main started\n");

    // Create Consumer
    pidtype cons_pid = create_process(consumer, NULL, "consumer");
    
    // Create Producer
    create_process(producer, (void*)(uintptr_t)cons_pid, "producer");

    // Main can exit or loop.
    // In kacchiOS if a process returns, it calls kill(self).
    serial_puts("[User] Main exiting (spawning complete)\n");
}
