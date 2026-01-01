#include "types.h"
#include "serial.h"
#include "idt.h"
#include "process.h"
#include "timer.h"
#include "heap.h"
#include "sem.h"

// Shared mutex for synchronization
extern void main(void* arg);

void kmain(void) {
    serial_init();
    serial_puts("\n--- kacchiOS Booting ---\n");
    
    idt_install();
    timer_init(); 
    heap_init();
    init_proc(); 
    sem_init();

    serial_puts("[Kernel] Spawning User Main...\n");
    create_process(main, NULL, "main");
    
    serial_puts("[Kernel] Starting multitasking...\n");
    run_null_process();
    while(1);
}
