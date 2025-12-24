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

    // Push argument for entry function
*(--sp) = (uintptr_t)arg;
    // Push fake return address (process_exit handler or 0)
    *(--sp) = 0;
    // When context switch restores, it should pop EIP from stack, so push entry
*(--sp) = (uintptr_t)entry;
    // Optionally push dummy EFLAGS, EBP, EBX, etc. (not strictly needed for minimal switch)
    // For now, just leave stack as is

    proc_table[pid].stackptr = sp;
    strcpy(proc_table[pid].name, name);// Will need to manually implement the strncpy,For now let's relax.
    proc_table[pid].name[15] = '\0';
return pid;
}

uint8_t current_pid = 0; // Global variable for current process

extern void context_switch(uintptr_t *old_sp, const uintptr_t *new_sp);
void switch_process(uint8_t next_pid) {
    if (next_pid == current_pid || next_pid >= NPROC) return;

    // Disable interrupts
    asm volatile ("cli");

    // Perform context switch
context_switch(&proc_table[current_pid].stackptr, (const uintptr_t *)&proc_table[next_pid].stackptr);

    // Update current process
    current_pid = next_pid;

    // Re-enable interrupts
    asm volatile ("sti");
}

