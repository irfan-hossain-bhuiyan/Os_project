# kacchiOS Features

## Version 2.0 - Enhanced Edition

This document provides a comprehensive overview of all features implemented in kacchiOS.

## 🚀 Core Features

### 1. Preemptive Multitasking

- **Process Management**: Up to 254 concurrent processes
- **Scheduler**: Round-robin with process aging
- **Context Switching**: Hardware-based, efficient switching
- **Time Quantum**: Configurable (default 20ms)
- **Anti-Starvation**: Aging mechanism prevents process starvation

### 2. Memory Management

#### Stack Allocation

- **Fixed-size stacks**: 4KB per process (i386 standard)
- **Pool-based allocator**: Fast allocation/deallocation
- **Capacity**: Up to 16 stacks
- **Tracking**: Usage statistics available

#### Heap Allocation

- **Dynamic allocation**: malloc/free/realloc support
- **Size**: 64KB heap space
- **Algorithm**: First-fit with coalescing
- **Alignment**: 16-byte alignment for performance
- **Features**:
  - Automatic block splitting
  - Free block coalescing
  - Fragmentation prevention
  - Usage statistics

### 3. Inter-Process Communication (IPC)

#### Message Passing

- **Model**: Asynchronous send, blocking receive
- **Capacity**: One message per process
- **Size**: 32-bit messages
- **Operations**:
  - `send(pid, msg)` - Send message to process
  - `receive()` - Block until message arrives

#### Semaphores

- **Count**: Up to 32 semaphores
- **Types**: Binary and counting semaphores
- **Operations**:
  - `sem_create(count)` - Create semaphore
  - `sem_wait(sem_id)` - P operation (blocks if count ≤ 0)
  - `sem_signal(sem_id)` - V operation (wakes waiting process)
  - `sem_delete(sem_id)` - Delete semaphore
- **Use Cases**:
  - Mutual exclusion
  - Synchronization
  - Resource management

### 4. Process States

- **PROC_FREE**: Slot available for new process
- **PROC_CURRENT**: Currently executing
- **PROC_READY**: Ready to run (in ready queue)
- **PROC_WAITING**: Blocked on semaphore
- **PROC_RECV**: Blocked waiting for message
- **PROC_TERMINATED**: Finished execution (pending cleanup)

### 5. Interrupt Handling

#### Interrupt Descriptor Table (IDT)

- **Size**: 256 entries
- **Type**: Protected mode IDT
- **Features**: Complete interrupt handling framework

#### Programmable Interrupt Controller (PIC)

- **Remapping**: IRQ0-15 mapped to INT 32-47
- **Masking**: Configurable interrupt masking
- **EOI**: Proper end-of-interrupt handling

#### Timer (PIT)

- **Chip**: Intel 8253/8254
- **Frequency**: ~50 Hz (20ms period)
- **Purpose**: Preemptive scheduling
- **Configurable**: Time quantum adjustable

## 🖥️ I/O Drivers

### 1. Serial Port Driver (COM1)

- **Baud Rate**: 115200 bps
- **Features**:
  - Character I/O
  - String output
  - Hexadecimal printing
  - Blocking read
- **Use**: Debug output, logging, interactive I/O

### 2. VGA Text Mode Driver

- **Resolution**: 80x25 characters
- **Colors**: 16 foreground, 8 background colors
- **Features**:
  - Character output with color
  - String output
  - Cursor control
  - Screen clearing
  - Scrolling
  - Hexadecimal and decimal printing
  - Tab, backspace, newline support
- **Memory**: Direct framebuffer access (0xB8000)

### 3. Keyboard Driver (PS/2)

- **Layout**: US QWERTY
- **Features**:
  - Interrupt-driven input
  - 128-byte circular buffer
  - Modifier key support (Shift, Ctrl, Alt)
  - Caps Lock support
  - Scancode to ASCII conversion
  - Blocking and non-blocking read
- **Special Keys**: All standard keys including function keys

## 📊 Monitoring & Debug Tools

### System Monitor

- **Process Table**: View all processes with status
- **Ready Queue**: Visualize scheduling queue
- **CPU Usage**: Per-process CPU time statistics
- **Memory Usage**: Heap and stack utilization
- **System Info**: Version, architecture, configuration
- **Process Dump**: Detailed information for specific process
- **Memory Dump**: Hexadecimal + ASCII memory viewer

### Debug Utilities

- **Serial Logging**: Comprehensive debug output
- **Hex Printing**: Memory and register inspection
- **Process Tracking**: Creation, switching, termination logs
- **Error Reporting**: Detailed error messages

## 🎯 System Calls

### Process Management

```c
pidtype create_process(proc_entry_t entry, void *arg, const char *name);
pidtype getpid(void);
int kill(pidtype pid);
```

### IPC - Messages

```c
int send(pidtype pid, uint32_t msg);
uint32_t receive(void);
```

### IPC - Semaphores

```c
int sem_create(int count);
int sem_wait(int sem_id);
int sem_signal(int sem_id);
int sem_delete(int sem_id);
```

### Memory Management

```c
void *malloc(unsigned int size);
void free(void *ptr);
void *realloc(void *ptr, unsigned int size);
```

## 🔧 Development Features

### Build System

- **Compiler**: GCC with i386 target
- **Assembler**: GNU as
- **Linker**: Custom linker script
- **Targets**:
  - `make` - Build kernel
  - `make run` - Run in QEMU (serial only)
  - `make run-vga` - Run with VGA display
  - `make debug` - Run with GDB support
  - `make test` - Run unit tests
  - `make clean` - Clean build artifacts

### Testing

- **Unit Tests**: Stack, heap, process manager
- **Test Framework**: Standalone test executables
- **Coverage**: Core memory and process functions

### Debugging

- **GDB Support**: Remote debugging via QEMU
- **Serial Output**: Comprehensive logging
- **Symbol Files**: Debug symbols in kernel.elf
- **Breakpoints**: Full source-level debugging

## 📈 Performance Characteristics

### Context Switch

- **Overhead**: ~100-500 microseconds
- **State Saved**: All general-purpose registers, flags, stack pointer
- **Optimization**: Minimal state saves

### Memory Allocation

- **malloc**: O(n) where n = number of blocks
- **free**: O(n) due to coalescing
- **Alignment**: 16 bytes (cache-line friendly)

### Scheduling

- **Decision Time**: O(n) where n = ready processes
- **Fairness**: Round-robin ensures fair CPU time
- **Aging**: Prevents starvation with O(n) age updates

## 🎓 Educational Value

### Learning Objectives

1. **Operating System Concepts**
   - Process lifecycle and states
   - Scheduling algorithms
   - Memory management
   - Inter-process communication

2. **Low-Level Programming**
   - Assembly language integration
   - Hardware interfacing
   - Memory-mapped I/O
   - Interrupt handling

3. **System Design**
   - Kernel architecture
   - API design
   - Resource management
   - Error handling

### Extensibility

The codebase is designed for easy extension:

- **Modular Structure**: Each component in separate files
- **Clear Interfaces**: Well-defined headers
- **Documentation**: Inline comments and external docs
- **Test Infrastructure**: Easy to add new tests

## 🔮 Future Enhancement Ideas

### Potential Extensions

1. **File System**: Simple FAT-like filesystem
2. **User Mode**: Ring 3 execution with syscalls
3. **Paging**: Virtual memory support
4. **Networking**: Simple network stack
5. **Shell**: More advanced command interpreter
6. **Pipes**: Unix-like pipes for IPC
7. **Signals**: Process signaling mechanism
8. **Priority Scheduling**: Multiple priority levels
9. **Real-Time Features**: Deadline scheduling
10. **Power Management**: CPU idle states

### Advanced Features

- **SMP Support**: Multi-core processing
- **64-bit Mode**: x86-64 architecture
- **UEFI Boot**: Modern boot process
- **Graphics**: Framebuffer graphics mode
- **Audio**: PC speaker or sound card
- **USB**: USB device support

## 📊 Statistics

- **Total Lines of Code**: ~3000+ (estimated)
- **Source Files**: 30+ files
- **Header Files**: 15+ headers
- **Test Files**: 3 comprehensive tests
- **Documentation**: 1000+ lines of documentation

## 🎉 Achievements

✅ Complete process management
✅ Full IPC implementation  
✅ Dynamic memory allocation
✅ Multiple I/O drivers
✅ Comprehensive monitoring tools
✅ Educational documentation
✅ Working interactive demo
✅ Extensive API coverage

kacchiOS 2.0 is a feature-complete educational operating system that demonstrates core OS concepts in a clean, understandable way!
