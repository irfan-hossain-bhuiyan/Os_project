#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"

#define NPROC 255

// Process states
enum proc_state {
    PROC_FREE = 0,
    PROC_CURRENT,
    PROC_READY,
    PROC_SUSPENDED,
    PROC_TERMINATED
};

struct procent {
    uint8_t pid;
    uint8_t state;
    uintptr_t *stackptr;
    void *stackbase;
    char name[16];
};

extern struct procent proc_table[NPROC];

typedef void (*proc_entry_t)(void *);
// Create a new process. Returns PID (0-254) or 255 on error.
uint8_t proc_create(proc_entry_t entry, const void *arg, const char *name);


extern void context_switch(uintptr_t *old_sp, uintptr_t *new_sp);

#endif // PROCESS_H
