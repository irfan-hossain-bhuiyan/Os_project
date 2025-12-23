# kacchiOS Development Plan

Rules for everybody
1. Remember there is no try/catch block in c,So errors must be returned,always prefer returning int for every function,and 
negative number is for errors, and 0 for ok.
2 . C don't have public and private variable.So everything that is public will be passed in header,
The header will be 
```
#ifndef HEADER_NAME
#define HEADER_NAME
//code
#endif
```
Also don't use int,float etc,Use iint_32_t,iint_64_t,etc,
Make a type alias for them.
WHy this ask llm.
## Phase 1: Foundation

### 1. Memory Manager (Basic)
- Implement a simple stack allocator for process stacks (4KB per process, i386 standard).
Add some test too.Will create a new folder for stack called `memory` and all the logic will be 
dumped there,Include some test as well,and a make file to see if it is working preferably.
- Provide basic heap allocation (fixed-size region).Bump allocator later.
- Functions: `alloc_stack(size)`, `free_stack(ptr)`, `alloc_heap(size)`, `free_heap(ptr)`
It is fine to use global variable,Just don't add them in header,
### 2. Process Manager (Basic)
- Define a `process_t` struct with:
  - PID (uint8_t, 0–254 valid, 255 invalid), state (CURRENT, READY,SUSPENDED ,TERMINATED), stack pointer, etc.
- Create a process table (array of 255 `process_t` entries, like Xinu).
- Implement process creation (`proc_create`), termination (`proc_terminate`), and state transitions.
THis implementation will be something like and array with index for empty array
on proc_create it will first a stack to check if it is empty,If it is allocate in current index and inc
If stack is non-empty pop,from it,(it contains the empty index).

### 3. Scheduler (Basic)

#### Implement a simple round-robin scheduler.
The sheduler will be a linkedlist,it will for now just contain process_id.there will be ready process linked list,suspended process linked list.

- Current process will just be a global variable.

#### Implement manual context switch: save/restore stack pointer and registers in software.

---

## Phase 2: Intermediate Features

### Memory Manager
- Add heap deallocation and optimize allocation (e.g., best-fit, free list).
- Track memory usage per process.

### Process Manager
- Add more process states (e.g., BLOCKED, WAITING).
- Implement utility functions: get current process, get process by PID, etc.
- Add basic IPC (message passing or shared memory).

### Scheduler
- Make time quantum configurable.
- Implement process aging to prevent starvation.

---

## Phase 3: Advanced Features

- Implement user mode and privilege separation.
- Add support for multiple heaps or memory regions.
- Advanced IPC (signals, pipes).
- Support for process priorities and advanced scheduling policies.

---

## Key Design Choices

- **Stack size:** 4KB per process (i386 standard)
- **Max processes:** 255 (like Xinu, using uint8_t PID)
- **Context switch:** Manual (software, not interrupt-based for now)
- **Scheduler:** Round-robin (each process gets a fixed time slice in a circular queue)

---

## Next Steps

1. Implement the process table and basic process struct.
2. Add stack allocation for new processes.
3. Write a simple scheduler and manual context switch.
4. Expand memory management and process features as needed.
