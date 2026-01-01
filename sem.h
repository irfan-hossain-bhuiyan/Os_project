#ifndef SEM_H
#define SEM_H

#include "types.h"
#include "process.h"

// Semaphore States
#define SEM_FREE 0
#define SEM_USED 1

struct sement {
    uint8_t state;      // SEM_FREE or SEM_USED
    int count;          // Semaphore count
    pidtype head;       // Head of waiting list (start)
    pidtype tail;       // Tail of waiting list (end)
};

#define NSEM 32 // Number of semaphores

extern struct sement sem_table[NSEM];

// Initialize semaphore system
void sem_init(void);

// Create a new semaphore with initial count
int sem_create(int count);

// Wait on a semaphore
int sem_wait(int sem_id);

// Signal a semaphore
int sem_signal(int sem_id);

// Delete/Free a semaphore
int sem_delete(int sem_id);

#endif // SEM_H
