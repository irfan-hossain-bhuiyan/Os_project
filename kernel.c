/* kernel.c - Main kernel with null process */
#include "types.h"
#include "serial.h"
#include "string.h"
#include "timer.h"
#include "idt.h"

#define MAX_INPUT 128

#include "process.h"

/* Test processes */
void process_a(void *arg) {
    while (1) {
        serial_puts(" [A] ");
        for (volatile int i = 0; i < 1000000; i++);
    }
}

void process_b(void *arg) {
    while (1) {
        serial_puts(" [B] ");
        for (volatile int i = 0; i < 1000000; i++);
    }
}

void kmain(void) {
    /* Initialize hardware */
    serial_init();
    
    /* Initialize IDT */
    idt_install();
    
    /* Create test processes */
    proc_create(process_a, "A", "proc_a"); // PID 0
    proc_create(process_b, "B", "proc_b"); // PID 1

    /* Initialize timer - This will eventually trigger the scheduler */
    timer_init();

    serial_puts("\nStarting kacchiOS Multitasking Test...\n");

    /* Jump to the first process manually for the first time */
    /* In a real kernel, the scheduler would handle this */
    switch_process(0);

    /* Should never reach here */
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
