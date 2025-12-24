#include "process.h"
#include "stack.h"
#include "string.h"

struct procent proc_table[NPROC];

// Helper to find a free PID
static int find_free_pid() {
    for (int i = 0; i < NPROC; ++i) {
        if (proc_table[i].state == PROC_FREE) return i;
    }
    return -1;
}

uint8_t proc_create(proc_entry_t entry, const void *arg, const char *name) {
    int pid = find_free_pid();
    if (pid < 0) return 255;
    void *stack = alloc_stack(4096);
    if (!stack) return 255;
    proc_table[pid].pid = pid;
    proc_table[pid].state = PROC_SUSPENDED;
    proc_table[pid].stackbase = stack;
    
    // i386 stack grows down, so start at top
    uintptr_t *sp = (uintptr_t*)((uint8_t*)stack + 4096);

    // --- C Calling Convention Setup ---
    *(--sp) = (uintptr_t)arg;      // Argument for entry function
    *(--sp) = 0;                   // Fake return address

    // --- context_switch Setup ---
    // When context_switch executes 'ret', it pops this:
    *(--sp) = (uintptr_t)entry;    // Entry point (EIP)
    
    // When context_switch executes 'popf', it pops this:
    *(--sp) = 0x202;               // EFLAGS (Interrupts enabled)

    // When context_switch executes 'popa', it pops 8 registers:
    for (int i = 0; i < 8; i++) {
        *(--sp) = 0;               // EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX
    }

    proc_table[pid].stackptr = sp;
    strcpy(proc_table[pid].name, name);
    proc_table[pid].name[15] = '\0';
    return pid;
}

uint8_t current_pid = 254; // Start at a dummy PID for kmain

extern void context_switch(uintptr_t **old_sp, uintptr_t **new_sp);
void switch_process(uint8_t next_pid) {
    if (next_pid == current_pid || next_pid >= NPROC) return;

    // Perform context switch
    uint8_t prev_pid = current_pid;
    current_pid = next_pid; // Update before switch so new task sees itself as current
    
    context_switch(&proc_table[prev_pid].stackptr, &proc_table[next_pid].stackptr);
}
