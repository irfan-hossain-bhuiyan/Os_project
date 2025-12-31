#include "types.h"
#include "serial.h"
#include "process.h"
#include "idt.h"
#include "timer.h"

void process_a(void *arg) {
    (void)arg;
    int count = 0;
    while (1) {
        serial_puts("[A:");
        serial_print_hex(count++);
        serial_puts("] ");
        for (volatile int i = 0; i < 2000000; i++);
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
    }
}

void kmain(void) {
    serial_init();
    serial_puts("\n--- kacchiOS Preemption Test (Round Robin) ---\n");
    
    idt_install();
    timer_init();
    init_proc(); 

    pidtype pid_a = create_process(process_a, NULL, "proc_a");
    pidtype pid_b = create_process(process_b, NULL, "proc_b");
    pidtype pid_c = create_process(process_c, NULL, "proc_c");

    (void)pid_a; (void)pid_b; (void)pid_c;
    
    serial_puts("Starting preemptive multitasking...\n");
    
    run_null_process();
    while(1);
}
