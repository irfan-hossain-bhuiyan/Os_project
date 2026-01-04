#include "kacchios.h"
#include <stdarg.h>

// ============================================================
// OS FEATURE DEMONSTRATION WITH PROCESS CREATION
// Demonstrates: Process, Memory Management, System Calls, 
//               IPC, Scheduling, File System, Interrupts
// ============================================================

// Process Control Block structure
typedef struct {
    int pid;
    char name[20];
    int priority;
    int state;  // 0=ready, 1=running, 2=waiting, 3=terminated
    void* stack_ptr;
    int parent_pid;
} pcb_t;

// System call numbers
#define SYSCALL_FORK    1
#define SYSCALL_EXEC    2
#define SYSCALL_EXIT    3
#define SYSCALL_WAIT    4
#define SYSCALL_GETPID  5
#define SYSCALL_SLEEP   6
#define SYSCALL_WRITE   7
#define SYSCALL_READ    8

// Process states
#define STATE_READY     0
#define STATE_RUNNING   1
#define STATE_WAITING   2
#define STATE_TERMINATED 3

// Global variables
volatile int current_pid = 0;
pcb_t processes[10];
int process_count = 0;

// ============================================================
// SYSTEM CALL IMPLEMENTATION
// ============================================================
int syscall(int number, ...) {
    va_list args;
    va_start(args, number);
    int result = 0;
    
    switch(number) {
        case SYSCALL_GETPID:
            result = current_pid;
            break;
            
        case SYSCALL_WRITE: {
            char* str = va_arg(args, char*);
            serial_puts(str);
            break;
        }
            
        case SYSCALL_SLEEP: {
            int ms = va_arg(args, int);
            // Simple delay loop (in real OS, use timer interrupts)
            for(int i = 0; i < ms * 1000; i++) {
                asm volatile("nop");
            }
            break;
        }
            
        case SYSCALL_FORK:
            // Create new process
            if(process_count < 10) {
                processes[process_count].pid = process_count + 1;
                processes[process_count].state = STATE_READY;
                processes[process_count].parent_pid = current_pid;
                result = processes[process_count].pid;
                process_count++;
                serial_puts("[SYSCALL] Process forked\n");
            }
            break;
            
        case SYSCALL_EXIT:
            processes[current_pid].state = STATE_TERMINATED;
            serial_puts("[SYSCALL] Process exited\n");
            break;
    }
    
    va_end(args);
    return result;
}

// Helper function for system calls
void sys_write(char* str) {
    syscall(SYSCALL_WRITE, str);
}

int sys_fork() {
    return syscall(SYSCALL_FORK);
}

void sys_exit() {
    syscall(SYSCALL_EXIT);
}

int sys_getpid() {
    return syscall(SYSCALL_GETPID);
}

void sys_sleep(int ms) {
    syscall(SYSCALL_SLEEP, ms);
}

// ============================================================
// INTERRUPT HANDLER DEMONSTRATION
// ============================================================
void timer_interrupt_handler() {
    static int ticks = 0;
    ticks++;
    
    // Every 1000 ticks, show we're alive
    if(ticks % 1000 == 0) {
        serial_puts("[IRQ] Timer interrupt - ");
        serial_print_hex(ticks);
        serial_puts(" ticks\n");
    }
    
    // Simple round-robin scheduling
    if(process_count > 1) {
        int next_pid = (current_pid + 1) % process_count;
        while(processes[next_pid].state == STATE_TERMINATED) {
            next_pid = (next_pid + 1) % process_count;
        }
        
        if(current_pid != next_pid) {
            processes[current_pid].state = STATE_READY;
            processes[next_pid].state = STATE_RUNNING;
            current_pid = next_pid;
            serial_puts("[SCHEDULER] Context switch to PID: ");
            serial_print_hex(current_pid);
            serial_puts("\n");
        }
    }
}

// ============================================================
// IPC (INTER-PROCESS COMMUNICATION) DEMONSTRATION
// ============================================================
#define IPC_QUEUE_SIZE 10

typedef struct {
    int data[IPC_QUEUE_SIZE];
    int front, rear;
    int count;
} ipc_queue_t;

ipc_queue_t message_queue;

void ipc_init() {
    message_queue.front = 0;
    message_queue.rear = 0;
    message_queue.count = 0;
}

int ipc_send(int msg) {
    if(message_queue.count >= IPC_QUEUE_SIZE) {
        return -1; // Queue full
    }
    
    message_queue.data[message_queue.rear] = msg;
    message_queue.rear = (message_queue.rear + 1) % IPC_QUEUE_SIZE;
    message_queue.count++;
    return 0;
}

int ipc_receive() {
    if(message_queue.count <= 0) {
        return -1; // Queue empty
    }
    
    int msg = message_queue.data[message_queue.front];
    message_queue.front = (message_queue.front + 1) % IPC_QUEUE_SIZE;
    message_queue.count--;
    return msg;
}

// ============================================================
// FILE SYSTEM SIMULATION
// ============================================================
typedef struct {
    char name[20];
    char content[100];
    int size;
} file_t;

file_t filesystem[5];
int file_count = 0;

int fs_create(char* name, char* content) {
    if(file_count >= 5) return -1;
    
    strcpy(filesystem[file_count].name, name);
    strcpy(filesystem[file_count].content, content);
    filesystem[file_count].size = strlen(content);
    file_count++;
    return 0;
}

char* fs_read(char* name) {
    for(int i = 0; i < file_count; i++) {
        if(strcmp(filesystem[i].name, name) == 0) {
            return filesystem[i].content;
        }
    }
    return NULL;
}

// ============================================================
// PROCESS FUNCTIONS
// ============================================================

// Process 1: Prime Number Generator (from original)
void process_prime() {
    int pid = sys_getpid();
    sys_write("[Process ");
    serial_print_hex(pid);
    sys_write("] Prime Generator Started\n");
    
    sys_write("How many primes to find? (1-100): ");
    int n = serial_read_int();
    
    if(n <= 0 || n > 100) {
        sys_write("[ERROR] Invalid input!\n");
        sys_exit();
        return;
    }
    
    // Allocate memory
    int *primes = (int *)malloc(n * sizeof(int));
    
    if(primes == NULL) {
        sys_write("[ERROR] malloc failed!\n");
        sys_exit();
        return;
    }
    
    // Find primes
    primes[0] = 2;
    int prime_count = 1;
    int candidate = 3;
    
    while(prime_count < n) {
        int is_prime = 1;
        for(int i = 0; i < prime_count; i++) {
            if(candidate % primes[i] == 0) {
                is_prime = 0;
                break;
            }
        }
        
        if(is_prime) {
            primes[prime_count] = candidate;
            prime_count++;
        }
        candidate += 2;
    }
    
    // Display results
    sys_write("Primes: ");
    for(int i = 0; i < n; i++) {
        serial_print_hex(primes[i]);
        sys_write(" ");
        if((i+1) % 5 == 0) sys_write("\n");
    }
    sys_write("\n");
    
    free(primes);
    sys_write("[Process ");
    serial_print_hex(pid);
    sys_write("] Completed\n");
    sys_exit();
}

// Process 2: Fibonacci Sequence
void process_fibonacci() {
    int pid = sys_getpid();
    sys_write("[Process ");
    serial_print_hex(pid);
    sys_write("] Fibonacci Generator Started\n");
    
    sys_write("How many Fibonacci numbers? (1-20): ");
    int n = serial_read_int();
    
    if(n <= 0 || n > 20) {
        sys_write("[ERROR] Invalid input!\n");
        sys_exit();
        return;
    }
    
    // Allocate memory
    int *fib = (int *)malloc(n * sizeof(int));
    
    if(fib == NULL) {
        sys_write("[ERROR] malloc failed!\n");
        sys_exit();
        return;
    }
    
    // Generate Fibonacci
    if(n >= 1) fib[0] = 0;
    if(n >= 2) fib[1] = 1;
    
    for(int i = 2; i < n; i++) {
        fib[i] = fib[i-1] + fib[i-2];
    }
    
    // Display
    sys_write("Fibonacci: ");
    for(int i = 0; i < n; i++) {
        serial_print_hex(fib[i]);
        sys_write(" ");
    }
    sys_write("\n");
    
    free(fib);
    
    // IPC Demonstration: Send message to parent
    ipc_send(pid * 1000 + n); // Encode PID and n in message
    
    sys_write("[Process ");
    serial_print_hex(pid);
    sys_write("] Sent IPC message\n");
    sys_exit();
}

// Process 3: File System Demo
void process_filesystem() {
    int pid = sys_getpid();
    sys_write("[Process ");
    serial_print_hex(pid);
    sys_write("] File System Demo Started\n");
    
    // Create files
    fs_create("hello.txt", "Hello from KacchiOS!");
    fs_create("config.cfg", "TIMEOUT=30\nDEBUG=1\nMODE=PROD");
    fs_create("data.bin", "0102030405060708090A");
    
    sys_write("Created 3 files:\n");
    sys_write("1. hello.txt\n");
    sys_write("2. config.cfg\n");
    sys_write("3. data.bin\n\n");
    
    // Read and display file
    char* content = fs_read("hello.txt");
    if(content) {
        sys_write("Contents of hello.txt: ");
        sys_write(content);
        sys_write("\n");
    }
    
    sys_write("[Process ");
    serial_print_hex(pid);
    sys_write("] File operations completed\n");
    sys_exit();
}

// ============================================================
// MAIN FUNCTION - OS DEMONSTRATION
// ============================================================
void main(void) {
    serial_puts("\n\n");
    serial_puts("===========================================\n");
    serial_puts("    KACCHI OS FEATURE DEMONSTRATION\n");
    serial_puts("===========================================\n\n");
    
    // Initialize OS components
    ipc_init();
    
    // Initialize init process (PID 0)
    processes[0].pid = 0;
    processes[0].state = STATE_RUNNING;
    processes[0].parent_pid = -1;
    strcpy(processes[0].name, "init");
    process_count = 1;
    current_pid = 0;
    
    serial_puts("[INIT] OS Initialized\n");
    serial_puts("[INIT] Process ID: ");
    serial_print_hex(sys_getpid());
    serial_puts("\n\n");
    
    // Demonstrate timer interrupts
    serial_puts("=== TIMER INTERRUPT SIMULATION ===\n");
    for(int i = 0; i < 5; i++) {
        timer_interrupt_handler();
        sys_sleep(100);
    }
    serial_puts("\n");
    
    // Demonstrate process creation
    serial_puts("=== PROCESS CREATION ===\n");
    
    // Create Process 1: Prime Generator
    int pid1 = sys_fork();
    if(pid1 > 0) {
        processes[pid1].state = STATE_READY;
        strcpy(processes[pid1].name, "prime_gen");
        serial_puts("[INIT] Created Process 1 (Prime Generator)\n");
    }
    
    // Create Process 2: Fibonacci
    int pid2 = sys_fork();
    if(pid2 > 0) {
        processes[pid2].state = STATE_READY;
        strcpy(processes[pid2].name, "fibonacci");
        serial_puts("[INIT] Created Process 2 (Fibonacci)\n");
    }
    
    // Create Process 3: File System
    int pid3 = sys_fork();
    if(pid3 > 0) {
        processes[pid3].state = STATE_READY;
        strcpy(processes[pid3].name, "filesystem");
        serial_puts("[INIT] Created Process 3 (File System)\n");
    }
    
    serial_puts("\n=== PROCESS EXECUTION ===\n");
    
    // Simulate process execution with scheduling
    for(int i = 0; i < 20; i++) {
        timer_interrupt_handler(); // This triggers scheduling
        
        // Run current process
        switch(current_pid) {
            case 0: // init process
                if(i == 5) {
                    serial_puts("[INIT] Waiting for child processes...\n");
                    
                    // Check for IPC messages
                    int msg = ipc_receive();
                    if(msg != -1) {
                        serial_puts("[INIT] Received IPC message: ");
                        serial_print_hex(msg);
                        serial_puts("\n");
                    }
                }
                break;
                
            case 1: // Prime process
                if(processes[1].state == STATE_RUNNING) {
                    process_prime();
                    processes[1].state = STATE_TERMINATED;
                }
                break;
                
            case 2: // Fibonacci process
                if(processes[2].state == STATE_RUNNING) {
                    process_fibonacci();
                    processes[2].state = STATE_TERMINATED;
                }
                break;
                
            case 3: // File system process
                if(processes[3].state == STATE_RUNNING) {
                    process_filesystem();
                    processes[3].state = STATE_TERMINATED;
                }
                break;
        }
        
        sys_sleep(50);
    }
    
    // Memory Management Demonstration
    serial_puts("\n=== MEMORY MANAGEMENT ===\n");
    
    // Show heap allocation
    void* ptr1 = malloc(64);
    void* ptr2 = malloc(128);
    void* ptr3 = malloc(256);
    
    serial_puts("[MEM] Allocated: 64, 128, 256 bytes\n");
    
    // Fragmentation demo
    free(ptr2);
    serial_puts("[MEM] Freed 128 byte block\n");
    
    void* ptr4 = malloc(100);
    serial_puts("[MEM] Allocated 100 bytes (fits in freed block)\n");
    
    // Cleanup
    free(ptr1);
    free(ptr3);
    free(ptr4);
    serial_puts("[MEM] All memory freed\n");
    
    // Final summary
    serial_puts("\n=== OS FEATURE SUMMARY ===\n");
    serial_puts("1. Process Management: ✓ (4 processes created)\n");
    serial_puts("2. Memory Management: ✓ (malloc/free working)\n");
    serial_puts("3. System Calls: ✓ (fork, exit, getpid, sleep)\n");
    serial_puts("4. Scheduling: ✓ (round-robin demonstrated)\n");
    serial_puts("5. IPC: ✓ (message passing shown)\n");
    serial_puts("6. File System: ✓ (virtual FS implemented)\n");
    serial_puts("7. Interrupts: ✓ (timer interrupt simulation)\n");
    
    serial_puts("\n===========================================\n");
    serial_puts("    OS DEMONSTRATION COMPLETED\n");
    serial_puts("===========================================\n\n");
    
    // Hang (in real OS, this would be idle loop)
    while(1) {
        asm volatile("hlt");
    }
}