#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"

#define NPROC (254)

typedef uint8_t pidtype;

struct ProcessNode
{
    pidtype before;
    pidtype after;
};
// Process states
enum proc_state
{
    PROC_FREE = 0,
    PROC_CURRENT,
    PROC_READY,
    PROC_WAITING,
    PROC_RECV,
    PROC_TERMINATED
};

struct Procent
{
    uint8_t pid;
    uint8_t state;
    uintptr_t *stackptr;
    void *stackbase;
    char name[16];

    // Message Passing
    uint32_t msg;
    int has_message;

    // Process aging (prevent starvation)
    uint32_t age;         // Incremented each tick while waiting
    uint32_t total_ticks; // Total CPU time consumed
};

extern struct Procent proc_table[NPROC];

typedef void (*proc_entry_t)(void *);
// Create a new process. Returns PID (0-15) or 255 on error.
pidtype create_process(proc_entry_t entry, const void *arg, const char *name);

// IPC
int send(pidtype pid, uint32_t msg);
uint32_t receive(void);

extern struct ProcessNode proc_nodes[NPROC];
void node_remove(pidtype pid);
void append_on_ready_list(pidtype pid);
pidtype get_next_node(pidtype pid);

extern uint8_t current_pid;
extern pidtype ready_list;

void init_proc(void);
void run_null_process(void);
void reshed(void);
void switch_process(pidtype);
int kill(pidtype pid);
#endif // PROCESS_H
