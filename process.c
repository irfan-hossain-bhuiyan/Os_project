#include "process.h"
#include "serial.h"
#include "stack.h"
#include "string.h"

struct procent proc_table[NPROC];
uint8_t current_pid = 255;
const size_t STACK_SIZE = 4096;

void null_process(void *arg) {
    (void)arg;
    serial_puts("[NULL] Null process is running!\n");
    while (1) {
        serial_puts(".");
        for (volatile int i = 0; i < 2000000; i++);
    }
}

void init_proc(void) {
    /* Initialize process table */
    for (int i = 0; i < NPROC; i++) {
        proc_table[i].state = PROC_FREE;
        proc_table[i].pid = i;
    }
    
    /* Create null process using proc_create */
    proc_create(null_process, NULL, "null_process");
}

uint8_t proc_create(proc_entry_t entry, const void *arg, const char *name) {
    /* Find a free slot */
    int pid = -1;
    for (int i = 0; i < NPROC; i++) {
        if (proc_table[i].state == PROC_FREE) {
            pid = i;
            break;
        }
    }
    
    if (pid == -1) return 255;
    
    void *stack = alloc_stack(STACK_SIZE);
    if (!stack) return 255;

    proc_table[pid].state = PROC_READY;
    proc_table[pid].stackbase = stack;
    
    /* Prepare initial stack frame */
    uintptr_t *sp = (uintptr_t *)((uint8_t *)stack + STACK_SIZE);
    
    *(--sp) = (uintptr_t)arg;           // Argument
    *(--sp) = 0;                        // Dummy Return Address
    *(--sp) = (uintptr_t)entry;         // Initial EIP
    *(--sp) = 0x002;                    // EFLAGS (Interrupts disabled)
    
    /* Push dummy registers for popa */
    for (int i = 0; i < 8; i++) {
        *(--sp) = 0;
    }
    
    proc_table[pid].stackptr = sp;
    strcpy(proc_table[pid].name, name);
    proc_table[pid].name[15] = '\0';
    
    return pid;
}

void run_null_process(void) {
    /* Switch to PID 0 (the null process created in init_proc) */
    switch_process(0);
}

void switch_process(uint8_t next_pid) {
    if (next_pid >= NPROC || proc_table[next_pid].state == PROC_FREE) return;

    /* Handle first-time switch from kernel to a process */
    if (current_pid == 255) {
        current_pid = next_pid;
        proc_table[next_pid].state = PROC_CURRENT;
        uintptr_t *sp = proc_table[next_pid].stackptr;

        __asm__ volatile(
            "mov %0, %%esp \n\t"
            "popa          \n\t"
            "popf          \n\t"
            "ret           \n\t"
            : 
            : "r" (sp)
            : "memory"
        );
        __builtin_unreachable();
    }

    if (next_pid == current_pid) return;

    uint8_t prev_pid = current_pid;
    current_pid = next_pid;
    
    proc_table[prev_pid].state = PROC_READY;
    proc_table[next_pid].state = PROC_CURRENT;
    
    /* 
     * Inline Context Switch 
     * We save current state and load the next state.
     * %0: pointer to old stack pointer (output)
     * %1: new stack pointer (input)
     */
    __asm__ volatile(
        "pushf          \n\t"
        "pusha          \n\t"
        "movl %%esp, %0 \n\t"  // Store current ESP into proc_table[prev].stackptr
        "movl %1, %%esp \n\t"  // Load new ESP from proc_table[next].stackptr
        "popa           \n\t"
        "popf           \n\t"
        : "=m" (proc_table[prev_pid].stackptr)
        : "m" (proc_table[next_pid].stackptr)
        : "memory"
    );
}
