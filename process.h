#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"

#define NPROC (254)

typedef uint8_t pidtype;
// Process states
enum proc_state {
    PROC_FREE = 0,
    PROC_CURRENT,
    PROC_READY,
    PROC_SUSPENDED,
    PROC_TERMINATED
};

struct Procent {
    uint8_t pid;
    uint8_t state;
    uintptr_t *stackptr;
    void *stackbase;
    char name[16];
};

extern struct Procent proc_table[NPROC];

typedef void (*proc_entry_t)(void *);
// Create a new process. Returns PID (0-15) or 255 on error.
pidtype create_process(proc_entry_t entry, const void *arg, const char *name);

extern uint8_t current_pid;
void init_proc(void);
void run_null_process(void);
void reshed(void);
#endif // PROCESS_H
