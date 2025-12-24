#include "types.h"
#include "serial.h"
#include "process.h"

void process_a(void *arg) {
    (void)arg;
    while (1) {
        serial_puts("A");
        for (volatile int i = 0; i < 2000000; i++);
        switch_process(1); // Switch to process B
    }
}

void process_b(void *arg) {
    (void)arg;
    while (1) {
        serial_puts("B");
        for (volatile int i = 0; i < 2000000; i++);
        switch_process(0); // Switch to process A
    }
}

void kmain(void) {
    serial_init();
    serial_puts("\n--- kacchiOS Hardcoded Context Switch Test ---\n");

    init_proc();
    
    serial_puts("Creating Process A...\n");
    proc_create(process_a, NULL, "proc_a"); // PID 0
    
    serial_puts("Creating Process B...\n");
    proc_create(process_b, NULL, "proc_b"); // PID 1

    serial_puts("Starting multitasking...\n");
    switch_process(0);

    /* Should never reach here */
    while(1);
}
