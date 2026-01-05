# API Reference

## Process Management

### create_process

```c
pidtype create_process(proc_entry_t entry, void *arg, const char *name);
```

Create a new process.

**Parameters:**

- `entry`: Function pointer to process entry point
- `arg`: Argument to pass to the process
- `name`: Process name (max 15 chars + null terminator)

**Returns:**

- PID of created process (0-254)
- 255 (INVALID_PID) on failure

**Example:**

```c
void my_process(void *arg) {
    // Process code here
}

pidtype pid = create_process(my_process, NULL, "my_process");
if (pid == 255) {
    // Handle error
}
```

### getpid

```c
pidtype getpid(void);
```

Get the current process ID.

**Returns:** Current process ID (0-254)

**Example:**

```c
pidtype my_pid = getpid();
serial_puts("My PID: ");
serial_print_hex(my_pid);
```

### kill

```c
int kill(pidtype pid);
```

Terminate a process.

**Parameters:**

- `pid`: Process ID to terminate (or self if current PID)

**Returns:**

- 0 on success
- -1 on failure (invalid PID)

**Example:**

```c
int result = kill(target_pid);
if (result != 0) {
    // Handle error
}
```

## Inter-Process Communication

### Message Passing

#### send

```c
int send(pidtype pid, uint32_t msg);
```

Send a 32-bit message to a process.

**Parameters:**

- `pid`: Target process ID
- `msg`: 32-bit message value

**Returns:**

- 0 on success
- -1 if invalid PID
- -2 if target's message buffer is full

**Example:**

```c
int result = send(target_pid, 0x12345678);
if (result == 0) {
    // Message sent successfully
} else if (result == -2) {
    // Receiver hasn't read previous message
}
```

#### receive

```c
uint32_t receive(void);
```

Receive a message. Blocks if no message is available.

**Returns:** 32-bit message value

**Example:**

```c
uint32_t msg = receive(); // Blocks until message arrives
serial_puts("Received: ");
serial_print_hex(msg);
```

### Semaphores

#### sem_create

```c
int sem_create(int count);
```

Create a semaphore with initial count.

**Parameters:**

- `count`: Initial semaphore count (typically 0 or 1)

**Returns:**

- Semaphore ID (0-31) on success
- -1 on failure (all semaphores in use)

**Example:**

```c
int sem = sem_create(0); // Binary semaphore
if (sem < 0) {
    // No semaphores available
}
```

#### sem_wait

```c
int sem_wait(int sem_id);
```

Wait on a semaphore (P operation). Blocks if count ≤ 0.

**Parameters:**

- `sem_id`: Semaphore ID (0-31)

**Returns:**

- 0 on success
- -1 on failure (invalid semaphore ID)

**Example:**

```c
sem_wait(sem); // Blocks until signaled
// Critical section
```

#### sem_signal

```c
int sem_signal(int sem_id);
```

Signal a semaphore (V operation). Wakes one waiting process.

**Parameters:**

- `sem_id`: Semaphore ID (0-31)

**Returns:**

- 0 on success
- -1 on failure (invalid semaphore ID)

**Example:**

```c
// Exit critical section
sem_signal(sem); // Wake one waiting process
```

#### sem_delete

```c
int sem_delete(int sem_id);
```

Delete a semaphore and free its ID.

**Parameters:**

- `sem_id`: Semaphore ID (0-31)

**Returns:**

- 0 on success
- -1 on failure (invalid semaphore ID)

**Example:**

```c
sem_delete(sem); // Free semaphore
```

## Memory Management

### malloc

```c
void *malloc(unsigned int size);
```

Allocate memory from the heap.

**Parameters:**

- `size`: Number of bytes to allocate

**Returns:**

- Pointer to allocated memory
- NULL on failure

**Example:**

```c
void *buffer = malloc(1024);
if (buffer == NULL) {
    // Out of memory
}
// Use buffer
free(buffer);
```

### free

```c
void free(void *ptr);
```

Free previously allocated memory.

**Parameters:**

- `ptr`: Pointer to memory to free (from malloc/realloc)

**Example:**

```c
void *buffer = malloc(1024);
// Use buffer
free(buffer); // Return memory to heap
```

### realloc

```c
void *realloc(void *ptr, unsigned int size);
```

Resize a previously allocated memory block.

**Parameters:**

- `ptr`: Pointer to existing allocation (or NULL)
- `size`: New size in bytes

**Returns:**

- Pointer to resized memory (may be different from original)
- NULL on failure (original allocation remains valid)

**Example:**

```c
void *buffer = malloc(512);
// Buffer too small, resize it
buffer = realloc(buffer, 1024);
if (buffer == NULL) {
    // Realloc failed, original buffer still valid
}
```

## Serial I/O

### serial_puts

```c
void serial_puts(const char *str);
```

Write a null-terminated string to serial port.

**Parameters:**

- `str`: String to write

**Example:**

```c
serial_puts("Hello from kacchiOS!\n");
```

### serial_print_hex

```c
void serial_print_hex(unsigned int value);
```

Print a value in hexadecimal format.

**Parameters:**

- `value`: Value to print

**Example:**

```c
serial_print_hex(0xDEADBEEF); // Output: 0xDEADBEEF
```

### serial_getchar

```c
char serial_getchar(void);
```

Read a single character from serial port (blocking).

**Returns:** Character read from serial port

**Example:**

```c
char c = serial_getchar();
serial_putchar(c); // Echo back
```

### serial_putchar

```c
void serial_putchar(char c);
```

Write a single character to serial port.

**Parameters:**

- `c`: Character to write

**Example:**

```c
serial_putchar('A');
```

## String Utilities

### strlen

```c
size_t strlen(const char *str);
```

Calculate string length.

**Parameters:**

- `str`: Null-terminated string

**Returns:** Length of string (excluding null terminator)

### strcmp

```c
int strcmp(const char *s1, const char *s2);
```

Compare two strings.

**Parameters:**

- `s1`: First string
- `s2`: Second string

**Returns:**

- 0 if strings are equal
- < 0 if s1 < s2
- > 0 if s1 > s2

### strcpy

```c
char *strcpy(char *dest, const char *src);
```

Copy a string.

**Parameters:**

- `dest`: Destination buffer
- `src`: Source string

**Returns:** Pointer to destination

### strncpy

```c
char *strncpy(char *dest, const char *src, size_t n);
```

Copy at most n characters of a string.

**Parameters:**

- `dest`: Destination buffer
- `src`: Source string
- `n`: Maximum number of characters to copy

**Returns:** Pointer to destination

### memset

```c
void *memset(void *ptr, int value, size_t num);
```

Fill memory with a constant byte.

**Parameters:**

- `ptr`: Pointer to memory
- `value`: Value to set (converted to unsigned char)
- `num`: Number of bytes to set

**Returns:** Pointer to memory

### memcpy

```c
void *memcpy(void *dest, const void *src, size_t n);
```

Copy memory block.

**Parameters:**

- `dest`: Destination pointer
- `src`: Source pointer
- `n`: Number of bytes to copy

**Returns:** Pointer to destination

## Error Codes

All functions returning `int` use the following error convention:

- **0**: Success
- **-1**: General error (invalid parameter, resource not found)
- **-2**: Resource full/busy
- **-3**: Resource exhausted

Special values:

- **255 (INVALID_PID)**: Invalid process ID

## Usage Patterns

### Producer-Consumer with Messages

```c
void producer(void *arg) {
    pidtype consumer_pid = (pidtype)(uintptr_t)arg;
    for (int i = 0; i < 10; i++) {
        send(consumer_pid, i);
    }
}

void consumer(void *arg) {
    for (int i = 0; i < 10; i++) {
        uint32_t msg = receive();
        // Process message
    }
}
```

### Mutex with Semaphore

```c
int mutex = sem_create(1); // Binary semaphore

void critical_section() {
    sem_wait(mutex);   // Lock
    // Critical section
    sem_signal(mutex); // Unlock
}
```

### Synchronization with Semaphore

```c
int sync_sem = sem_create(0);

void worker(void *arg) {
    // Do work
    sem_signal(sync_sem); // Signal completion
}

void main_process(void *arg) {
    create_process(worker, NULL, "worker");
    sem_wait(sync_sem); // Wait for worker to complete
    // Continue after worker done
}
```
