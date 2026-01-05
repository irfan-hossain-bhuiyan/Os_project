# kacchiOS Architecture

## System Overview

kacchiOS is a preemptive, multitasking operating system designed for the x86 architecture. It implements core OS concepts including process management, memory management, scheduling, and inter-process communication.

## Core Components

### 1. Boot Process

```text
GRUB/Multiboot
    ↓
boot.S (Assembly bootstrap)
    ↓
kmain() (Kernel initialization)
    ↓
Hardware initialization (Serial, IDT, Timer, PIC)
    ↓
Memory initialization (Heap)
    ↓
Process initialization
    ↓
Create user main process
    ↓
Start null process (scheduler)
```

### 2. Memory Management

#### Stack Allocation

- **Fixed Size**: 4KB per process (i386 standard)
- **Allocator**: Simple stack-based allocator
- **Storage**: Array-based with free list using stack data structure
- **Range**: Processes reuse freed stack slots

#### Heap Allocation

- **Algorithm**: Bump allocator with free list
- **Functions**: `malloc()`, `free()`, `realloc()`
- **Features**:
  - Dynamic memory allocation
  - Coalescing of free blocks
  - Memory tracking and debugging

#### Memory Layout

```text
+------------------+  0xFFFFFFFF
|   Kernel Code    |
+------------------+
|   Kernel Data    |
+------------------+
|   Heap (grows →) |
+------------------+
|   Process Stacks |
|   (4KB each)     |
+------------------+
|   Free Memory    |
+------------------+  0x00000000
```

### 3. Process Management

#### Process Structure

```c
typedef struct {
    uint8_t pid;           // Process ID (0-254, 255=invalid)
    uint8_t state;         // CURRENT, READY, SUSPENDED, TERMINATED
    void *stack_ptr;       // Saved stack pointer
    void *stack_base;      // Stack base address
    uint32_t msg;          // Message buffer
    uint8_t has_msg;       // Message available flag
    char name[16];         // Process name
} process_t;
```

#### Process States

- **CURRENT**: Currently executing
- **READY**: Ready to run (in ready queue)
- **SUSPENDED**: Blocked on semaphore or message
- **TERMINATED**: Finished execution

#### Process Table

- **Size**: 255 entries (PID 0-254)
- **Management**: Array-based with free list stack
- **Reuse**: PIDs are recycled when processes terminate

### 4. Scheduling

#### Algorithm: Round-Robin

- **Policy**: Cooperative + Preemptive
- **Time Quantum**: 20ms (configurable at boot)
- **Queue**: Circular linked list of ready processes
- **Context Switch**: Hardware-based (timer interrupt)

#### Scheduler Flow

```text
Timer Interrupt (every 20ms)
    ↓
Save current process context
    ↓
Move current → ready queue
    ↓
Dequeue next ready process
    ↓
Restore next process context
    ↓
Resume execution
```

### 5. Inter-Process Communication

#### Message Passing

- **Model**: Asynchronous send, blocking receive
- **Buffer**: Single message per process
- **Functions**:
  - `send(pid, msg)` - Send 32-bit message
  - `receive()` - Block until message arrives

#### Semaphores

- **Count**: 32 semaphores available
- **Operations**:
  - `sem_create(count)` - Create semaphore
  - `sem_wait(sem_id)` - P operation (blocks if count ≤ 0)
  - `sem_signal(sem_id)` - V operation (wakes one waiter)
  - `sem_delete(sem_id)` - Delete semaphore

### 6. Interrupt Handling

#### IDT (Interrupt Descriptor Table)

- **Size**: 256 entries
- **Type**: Protected mode IDT
- **Handlers**: Timer, keyboard, system calls

#### PIC (Programmable Interrupt Controller)

- **Remapping**: IRQ0-15 → INT 32-47
- **Masking**: Configurable interrupt masking

#### Timer

- **Chip**: Intel 8253/8254 PIT
- **Frequency**: ~50 Hz (20ms period)
- **Purpose**: Preemptive scheduling

### 7. System Calls

System calls provide user-level API for kernel services:

```c
// Process management
pidtype create_process(proc_entry_t entry, void *arg, const char *name);
pidtype getpid(void);
int kill(pidtype pid);

// IPC - Messages
int send(pidtype pid, uint32_t msg);
uint32_t receive(void);

// IPC - Semaphores
int sem_create(int count);
int sem_wait(int sem_id);
int sem_signal(int sem_id);
int sem_delete(int sem_id);

// Memory
void *malloc(unsigned int size);
void free(void *ptr);
void *realloc(void *ptr, unsigned int size);
```

## Design Decisions

### Why 255 Processes?

- **PID Size**: `uint8_t` (0-255)
- **Special Value**: 255 = INVALID_PID
- **Usable Range**: 0-254 (255 processes)
- **Inspiration**: Xinu OS design

### Why 4KB Stack?

- **Standard**: i386 architecture standard
- **Sufficient**: Adequate for most embedded tasks
- **Alignment**: Page-aligned (future paging support)

### Why Round-Robin?

- **Simplicity**: Easy to understand and implement
- **Fairness**: Each process gets equal CPU time
- **Educational**: Clear demonstration of scheduling concepts

### Why 20ms Time Quantum?

- **Historical**: Early Linux default
- **Balance**: Good tradeoff between responsiveness and overhead
- **Configurable**: Can be adjusted at kernel init

## Performance Considerations

### Context Switch Overhead

- **Time**: ~100-500 microseconds
- **Saved State**: Registers, stack pointer, process state
- **Optimization**: Minimal state saves, efficient queue operations

### Memory Efficiency

- **Stack**: Fixed 4KB, no waste
- **Heap**: Coalescing prevents fragmentation
- **Process Table**: Static allocation, no runtime overhead

### Scalability

- **Current**: 255 processes
- **Future**: Could extend with hierarchical PIDs or 16-bit PIDs

## Security Considerations

### Current State

- **No Privilege Separation**: All code runs in kernel mode
- **No Memory Protection**: Processes can access all memory
- **No Input Validation**: Minimal syscall parameter checking

### Future Improvements

- **User Mode**: Implement ring 3 execution
- **Paging**: Enable memory protection
- **Validation**: Add comprehensive syscall parameter validation

## Debugging Support

- **Serial Output**: Comprehensive logging via COM1
- **GDB Support**: Remote debugging via QEMU
- **Debug Macros**: Conditional debug output
- **Hex Printing**: Memory and register inspection

## References

- [XINU OS Design](https://xinu.cs.purdue.edu/)
- [OSDev Wiki](https://wiki.osdev.org/)
- [Intel® 64 and IA-32 Architectures Software Developer's Manual](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
