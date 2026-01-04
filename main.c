// kacchiOS feature showcase main
// This user-level main is created as a process by kmain()
// and demonstrates:
//   - Process creation & scheduling
//   - Message passing (send/receive)
//   - Semaphores (sem_create / sem_wait / sem_signal)
//   - Heap allocation (malloc / free)

#include "kacchios.h"

// Global semaphore used to let main() know when
// the messaging demo has completed.
static int done_sem;

// Consumer process: receives a fixed number of messages
// from a producer, then signals the "done" semaphore.
void consumer(void *arg) {
    (void)arg; // Unused for now; could carry sem id if desired

    serial_puts("[consumer] PID=");
    serial_print_hex(getpid());
    serial_puts(": waiting for messages...\n");

    const int total_msgs = 5;
    for (int i = 0; i < total_msgs; i++) {
        uint32_t msg = receive();
        serial_puts("[consumer] received msg=");
        serial_print_hex(msg);
        serial_puts("\n");
    }

    serial_puts("[consumer] all messages received, signaling main via semaphore.\n");
    sem_signal(done_sem);

    // Optionally exercise the heap in this process too
    void *ptr = malloc(128);
    if (ptr) {
        serial_puts("[consumer] heap alloc 128 bytes OK, freeing...\n");
        free(ptr);
    } else {
        serial_puts("[consumer] heap alloc FAILED\n");
    }

    // Keep the process around so scheduler can continue to run it if desired
    while (1) {
        // Busy wait to keep the process alive and schedulable
    }
}

// Producer process: sends a sequence of integer messages
// to the consumer using the message-passing API.
void producer(void *arg) {
    pidtype target_pid = (pidtype)(uintptr_t)arg;

    serial_puts("[producer] PID=");
    serial_print_hex(getpid());
    serial_puts(" sending to PID=");
    serial_print_hex(target_pid);
    serial_puts("\n");

    const int total_msgs = 5;
    for (int i = 0; i < total_msgs; i++) {
        // Simple delay loop so output interleaves and
        // you can see the scheduler at work.
        for (volatile int j = 0; j < 5000000; j++) {
        }

        serial_puts("[producer] sending msg=");
        serial_print_hex(i);
        serial_puts("\n");

        int res = send(target_pid, (uint32_t)i);
        if (res != 0) {
            serial_puts("[producer] send FAILED\n");
        }
    }

    serial_puts("[producer] finished sending messages.\n");

    while (1) {
        // Keep process alive
    }
}

// Optional extra worker that only exercises the heap API
// to clearly demonstrate malloc/free from a separate process.
void heap_worker(void *arg) {
    (void)arg;

    serial_puts("[heap_worker] PID=");
    serial_print_hex(getpid());
    serial_puts(" performing heap allocations...\n");

    const int count = 4;
    const size_t size = 64;
    void *blocks[count];

    for (int i = 0; i < count; i++) {
        blocks[i] = malloc(size);
        serial_puts("[heap_worker] alloc #");
        serial_print_hex(i);
        serial_puts(" -> ");
        serial_print_hex((uintptr_t)blocks[i]);
        serial_puts("\n");
    }

    for (int i = 0; i < count; i++) {
        if (blocks[i]) {
            free(blocks[i]);
        }
    }

    serial_puts("[heap_worker] allocations freed.\n");

    while (1) {
        // Idle forever
    }
}

// User-level entry created by kmain() as a process.
// It orchestrates the demo by creating additional processes
// and using semaphores to wait for completion.
void main(void *arg) {
    (void)arg;

    serial_puts("\n\n");
    serial_puts("===========================================\n");
    serial_puts("  kacchiOS Feature Showcase\n");
    serial_puts("  - Processes & Scheduling\n");
    serial_puts("  - Message Passing (send/receive)\n");
    serial_puts("  - Semaphores (P/V)\n");
    serial_puts("  - Heap (malloc/free)\n");
    serial_puts("===========================================\n\n");

    serial_puts("[main] PID=");
    serial_print_hex(getpid());
    serial_puts(" starting demo...\n");

    // Create a semaphore with initial count 0.
    // main() will block on this until the consumer signals it,
    // demonstrating semaphore-based synchronization between processes.
    done_sem = sem_create(0);
    if (done_sem < 0) {
        serial_puts("[main] ERROR: failed to create semaphore.\n");
        return;
    }

    // Spawn the consumer first so we know which PID to send to.
    pidtype cons_pid = create_process(consumer, NULL, "consumer");
    serial_puts("[main] created consumer PID=");
    serial_print_hex(cons_pid);
    serial_puts("\n");

    // Spawn the producer, passing the consumer's PID as its argument.
    create_process(producer, (void *)(uintptr_t)cons_pid, "producer");
    serial_puts("[main] created producer targeting consumer.\n");

    // Spawn an independent worker that only exercises heap usage.
    create_process(heap_worker, NULL, "heap_worker");
    serial_puts("[main] created heap_worker.\n");

    serial_puts("[main] waiting on semaphore from consumer...\n");
    sem_wait(done_sem);

    serial_puts("[main] semaphore signaled! messaging demo complete.\n");

    // Clean up semaphore now that we're done with it.
    sem_delete(done_sem);

    // Also exercise the heap directly from main.
    serial_puts("[main] testing heap from main process...\n");
    void *buf = malloc(256);
    if (buf) {
        serial_puts("[main] malloc(256) OK, freeing...\n");
        free(buf);
    } else {
        serial_puts("[main] malloc(256) FAILED\n");
    }

    serial_puts("[main] demo complete. main process will now idle.\n");

    // Keep main alive so you can continue to see scheduling effects.
    while (1) {
        // Idle loop; other processes continue to run.
    }
}
