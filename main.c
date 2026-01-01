#include "kacchios.h"

void process_a(void* arg) {
    (void)arg;
    int index = 0;
    while(1) {
        serial_puts("Process A: ");
        serial_print_hex(index++);
        serial_puts("\n");
        // Busy wait
        for(volatile int i=0; i<10000000; i++);
    }
}

void process_b(void* arg) {
    (void)arg;
    int index = 0;
    while(1) {
        serial_puts("Process B: ");
        serial_print_hex(index++);
        serial_puts("\n");
        // Busy wait
        for(volatile int i=0; i<10000000; i++);
    }
}

void main(void) {
    serial_puts("\n[User] Main: Creating Process A and B...\n");
    
    create_process(process_a, NULL, "proc_a");
    create_process(process_b, NULL, "proc_b");
    
    serial_puts("[User] Main: Exiting...\n");
}
