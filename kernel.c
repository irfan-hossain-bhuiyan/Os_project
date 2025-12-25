#include "types.h"
#include "serial.h"
#include "process.h"
#include "idt.h"
void process_a(void *arg) {
    (void)arg;
    int count = 0;
    while (1) {
        serial_puts("[A:");
        serial_print_hex(count++);
        serial_puts("] ");
        for (volatile int i = 0; i < 2000000; i++);
	reshed();
    }
}

void process_b(void *arg) {
    (void)arg;
    int count = 0;
    while (1) {
        serial_puts("[B:");
        serial_print_hex(count++);
        serial_puts("] ");
        for (volatile int i = 0; i < 2000000; i++);
	reshed();
    }
}

void process_c(void *arg) {
    (void)arg;
    int count = 0;
    while (1) {
        serial_puts("[C:");
        serial_print_hex(count++);
        serial_puts("] ");
        for (volatile int i = 0; i < 2000000; i++);
	reshed();
    }
}

void kmain(void) {
    serial_init();
    serial_puts("\n--- kacchiOS Multi-Process State Test ---\n");
    // Install the Interrupt Descriptor Table (IDT), including the triple fault handler at vector 8.
    idt_install();
    init_proc(); // Creates Null Process at PID 0
    
    serial_puts("Creating Process A (PID 1)...\n");
    create_process(process_a, NULL, "proc_a"); 
    
    serial_puts("Creating Process B (PID 2)...\n");
    create_process(process_b, NULL, "proc_b"); 

    serial_puts("Creating Process C (PID 3)...\n");
    create_process(process_c, NULL, "proc_c"); 
    
    serial_puts("Starting multitasking at A...\n");
    run_null_process();
    /* Should never reach here */
    while(1);
}
