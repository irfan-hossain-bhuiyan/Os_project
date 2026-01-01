#ifndef KACCHIOS_H
#define KACCHIOS_H

#include "types.h"

// Define pidtype (matches process.h)
typedef uint8_t pidtype;

// --- Process Management ---
typedef void (*proc_entry_t)(void *);

// Create a new process
// Returns: PID of created process, or 255 on failure
pidtype create_process(proc_entry_t entry, const void *arg, const char *name);

// Get current Process ID
pidtype getpid(void);

// Terminate a process (or self)
int kill(pidtype pid);

// --- IPC: Message Passing ---

// Send a 32-bit message to a process.
// Returns: 0 on success, -1 (invalid pid), -2 (buffer full)
int send(pidtype pid, uint32_t msg);

// Receive a 32-bit message. Blocks if no message is available.
uint32_t receive(void);

// --- IPC: Semaphores ---

// Create a semaphore with initial count
// Returns: Semaphore ID (0-31) or -1 on failure
int sem_create(int count);

// Wait (P) on a semaphore. Blocks if count <= 0.
int sem_wait(int sem_id);

// Signal (V) a semaphore. Wakes one waiter.
int sem_signal(int sem_id);

// Delete a semaphore and free it.
int sem_delete(int sem_id);

// --- Memory Management ---

void *malloc(unsigned int size);
void free(void *ptr);
void *realloc(void *ptr, unsigned int size);

// --- I/O & Utils ---

void serial_puts(const char *str);
void serial_print_hex(uint32_t val);

// Standard string functions (if needed by user)
size_t strlen(const char *str);
char *strcpy(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);

#endif // KACCHIOS_H
