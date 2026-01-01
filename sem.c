#include "sem.h"
#include "process.h" /* for proc_nodes, node_remove, append_on_ready_list */
#include "serial.h"
#include "debug.h"

struct sement sem_table[NSEM];

void sem_init(void) {
    for (int i = 0; i < NSEM; i++) {
        sem_table[i].state = SEM_FREE;
        sem_table[i].count = 0;
        sem_table[i].head = 255;
        sem_table[i].tail = 255;
    }
    kdebug_puts("[SEM] Semaphore system initialized (Linear Lists)\n");
}

int sem_create(int count) {
    __asm__ volatile("cli");
    for (int i = 0; i < NSEM; i++) {
        if (sem_table[i].state == SEM_FREE) {
            sem_table[i].state = SEM_USED;
            sem_table[i].count = count;
            sem_table[i].head = 255;
            sem_table[i].tail = 255;
            __asm__ volatile("sti");
            return i;
        }
    }
    __asm__ volatile("sti");
    return -1;
}

int sem_wait(int sem_id) {
    __asm__ volatile("cli");
    
    if (sem_id < 0 || sem_id >= NSEM || sem_table[sem_id].state == SEM_FREE) {
        __asm__ volatile("sti");
        return -1;
    }

    sem_table[sem_id].count--;

    if (sem_table[sem_id].count < 0) {
        // Log block
        // kdebug_puts("[SEM] Blocking PID ");
        // kdebug_puthex(current_pid);
        // kdebug_puts("\n");

        // Set state first
        proc_table[current_pid].state = PROC_WAITING;
        
        // Remove from Ready List (Circular)
        node_remove(current_pid);

        // Append to Semaphore Linear List using proc_nodes
        // Note: node_remove leaves .before/.after pointers arguably dirty if we reuse them differently
        // But for linear list we only strictly need .after.
        
        proc_nodes[current_pid].after = 255; // End of list marker
        
        if (sem_table[sem_id].tail == 255) {
            // List was empty
            sem_table[sem_id].head = current_pid;
            sem_table[sem_id].tail = current_pid;
        } else {
            // Append to tail
            proc_nodes[sem_table[sem_id].tail].after = current_pid;
            sem_table[sem_id].tail = current_pid;
        }

        // Reschedule
        reshed();
    }

    __asm__ volatile("sti");
    return 0;
}

int sem_signal(int sem_id) {
    __asm__ volatile("cli");

    if (sem_id < 0 || sem_id >= NSEM || sem_table[sem_id].state == SEM_FREE) {
        __asm__ volatile("sti");
        return -1;
    }

    sem_table[sem_id].count++;

    if (sem_table[sem_id].count <= 0) {
        // There are waiters. Wake the head.
        pidtype pid = sem_table[sem_id].head;
        
        if (pid == 255) {
            // Logic error: count implies waiters but list is empty?
            klog_error("sem_signal: count negative but list empty");
            __asm__ volatile("sti");
            return -1;
        }

        // Dequeue Head
        sem_table[sem_id].head = proc_nodes[pid].after;
        if (sem_table[sem_id].head == 255) {
            // List became empty
            sem_table[sem_id].tail = 255;
        }

        // Make Ready
        proc_table[pid].state = PROC_READY;
        append_on_ready_list(pid);
    }

    __asm__ volatile("sti");
    return 0;
}

int sem_delete(int sem_id) {
     __asm__ volatile("cli");
      if (sem_id < 0 || sem_id >= NSEM) {
         __asm__ volatile("sti");
         return -1;
     }
     
     // Free all waiting processes (move them to ready)
     while (sem_table[sem_id].head != 255) {
         pidtype pid = sem_table[sem_id].head;
         
         // Dequeue logic inline
         sem_table[sem_id].head = proc_nodes[pid].after;
         if (sem_table[sem_id].head == 255) sem_table[sem_id].tail = 255;

         proc_table[pid].state = PROC_READY;
         append_on_ready_list(pid);
     }
     
     sem_table[sem_id].state = SEM_FREE;
     __asm__ volatile("sti");
     return 0;
}
