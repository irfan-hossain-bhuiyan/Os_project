#include "process.h"
#include "serial.h"
#include "stack.h"
#include "string.h"
#include "system.h"
#include "debug.h"

void switch_process(pidtype next_pid);

struct Procent proc_table[NPROC];
struct ProcessNode proc_nodes[NPROC];

// Ready list head PID (255 means empty)
pidtype ready_list = 255;
static pidtype proc_create(proc_entry_t entry, const void *arg, const char *name);

pidtype get_next_node(pidtype pid) {
    return proc_nodes[pid].after;
}

pidtype get_previous_node(pidtype pid) {
    return proc_nodes[pid].before;
}

static void node_append_before(pidtype pid, pidtype move_before) {
  pidtype before = get_previous_node(move_before);
  proc_nodes[pid].after = move_before;
  proc_nodes[pid].before = before;
  proc_nodes[before].after = pid;
  proc_nodes[move_before].before = pid;
}

// remove a node from the linked list
static void node_remove(pidtype pid) {
    pidtype prev = proc_nodes[pid].before;
    pidtype next = proc_nodes[pid].after;

    if (pid == next) {
        // Only one element in the list
        ready_list = 255;
    } else {
        proc_nodes[prev].after = next;
        proc_nodes[next].before = prev;
        
        // Update ready_list head if we removed the head
        if (ready_list == pid) {
            ready_list = next;
        }
    }
}

uint8_t current_pid = 255;
const size_t STACK_SIZE = 4096;

pidtype getpid(void) {
    return current_pid;
}

// Round-robin with lazy zombie cleanup
static void switch_to_next_process(void) {
  if (ready_list == 255) return;

  // Start checking from the next node
  pidtype curr = get_next_node(current_pid);
  if (curr == 255) curr = ready_list; // Fallback if current_pid wasn't in list

  pidtype start_check = curr;
  
  while (1) {
      if (proc_table[curr].state == PROC_TERMINATED) {
          // Process is terminated, proceed with cleanup
          
          // Safety: Don't clean our own stack while running on it
          if (curr != current_pid) {
              pidtype to_clean = curr;
              curr = get_next_node(curr); // Advance curr before unlinking
              
              node_remove(to_clean);
              free_stack(proc_table[to_clean].stackbase);
              proc_table[to_clean].state = PROC_FREE;
              
              kdebug_puts("[INFO] Cleanup complete for PID ");
              // kdebug_puthex(to_clean);
              kdebug_puts("\n");
              
              if (ready_list == 255) return; // List became empty
              // If we looped back to start because of removal, update start
              if (to_clean == start_check) start_check = curr;
              
              continue; // Continue loop with new curr
          }
      }

      if (proc_table[curr].state == PROC_READY) {
          switch_process(curr);
          return;
      }

      curr = get_next_node(curr);
      
      // Full circle check
      if (curr == start_check) {
          // No ready process found (all might be suspended or current is the only one)
          return;
      }
  }
}

void reshed(void) {
    //kdebug_puts("\n[DEBUG] reshed called\n");
    __asm__ volatile("cli");
    switch_to_next_process();
    __asm__ volatile("sti");
}

static void append_on_ready_list(pidtype pid) {
  if (pid == 255)
    return;
  if (ready_list == 255) {
    // List is empty, initialize single-node circle
    ready_list = pid;
    proc_nodes[pid].before = pid;
    proc_nodes[pid].after = pid;
  } else {
    // Insert before the current head (ready_list)
    node_append_before(pid, ready_list);
    ready_list = pid;
  }
}

static void null_process(void *arg) {
  (void)arg;
  while(1) reshed();
}

static void init_proc_table() {
  for (int i = 0; i < NPROC; i++) {
    proc_table[i].state = PROC_FREE;
    proc_table[i].pid = i;
  }
}

static void init_proc_nodes() {
  for (int i = 0; i < NPROC; i++) {
    proc_nodes[i].after = i;
    proc_nodes[i].before = i;
  }
}

void init_proc(void) {
  init_proc_nodes();
  init_proc_table();
  create_process(null_process, NULL, "null_process");
}

// Wrapper: Create a process and append it to the ready list
pidtype create_process(proc_entry_t entry, const void *arg, const char *name) {
  kdebug_puts("[INFO] create_process: ");
  kdebug_puts(name);
  kdebug_puts("\n");
  uint8_t pid = proc_create(entry, arg, name);
  if (pid != 255) {
    append_on_ready_list(pid);
  }
  return pid;
}

int kill(pidtype pid) {
    if (pid == 0) {
        klog_error("kill: null_process can't be terminated");
        return -1;
    }
    
    if (pid >= NPROC || proc_table[pid].state == PROC_FREE) {
        return -1;
    }

    proc_table[pid].state = PROC_TERMINATED;
    reshed();
    return 0;
}

// Safety net: called if a process mistakenly returns.
void on_process_end(void) {
    kill(getpid());
}

static pidtype proc_create(proc_entry_t entry, const void *arg, const char *name) {
  /* Find a free slot */
  int pid = -1;
  for (int i = 0; i < NPROC; i++) {
    if (proc_table[i].state == PROC_FREE) {
      pid = i;
      break;
    }
  }

  if (pid == -1) {
    klog_error("proc_create: no free process slots");
    return 255;
  }

  void *stack = alloc_stack(STACK_SIZE);
  if (!stack) {
    klog_error("proc_create: stack allocation failed");
    return 255;
  }

  proc_table[pid].state = PROC_READY;
  proc_table[pid].stackbase = stack;

  /* Prepare initial stack frame */
  uintptr_t *sp = (uintptr_t *)((uint8_t *)stack + STACK_SIZE);

  *(--sp) = (uintptr_t)arg;           // Argument
  *(--sp) = (uintptr_t)on_process_end; // RETURN ADDRESS (Safety Net)
  *(--sp) = (uintptr_t)entry;          // Initial EIP
  *(--sp) = 0x202;                     // EFLAGS (Interrupts enabled)

  /* Push dummy registers for popa */
  for (int i = 0; i < 8; i++) {
    *(--sp) = 0;
  }

  proc_table[pid].stackptr = sp;
  strcpy(proc_table[pid].name, name);
  proc_table[pid].name[15] = '\0';

  return pid;
}

void run_null_process(void) {
  kdebug_puts("[INFO] run_null_process: jumping to PID 0\n");
  /* Switch to PID 0 (the null process created in init_proc) */
  switch_process(0);
}

void switch_process(pidtype next_pid) {
  if (next_pid >= NPROC || proc_table[next_pid].state == PROC_FREE) {
    klog_error("switch_process: target PID is invalid or FREE");
    return;
  }

  /* Handle first-time switch from kernel to a process */
  if (current_pid == 255) {
    current_pid = next_pid;
    proc_table[next_pid].state = PROC_CURRENT;
    uintptr_t *sp = proc_table[next_pid].stackptr;

    __asm__ volatile("mov %0, %%esp \n\t"
                     "popa          \n\t"
                     "popf          \n\t"
                     "ret           \n\t"
                     :
                     : "r"(sp)
                     : "memory");
    __builtin_unreachable();
  }

  if (next_pid == current_pid)
    return;

  uint8_t prev_pid = current_pid;
  current_pid = next_pid;

  // Important: logic modification to support termination
  // Only set PREV to READY if it is still CURRENT (meaning it yielded or was preempted alive)
  // If kill() was called, state is already PROC_TERMINATED. don't overwrite it.
  if (proc_table[prev_pid].state == PROC_CURRENT) {
      proc_table[prev_pid].state = PROC_READY;
  }

  proc_table[next_pid].state = PROC_CURRENT;

  /*
   * Inline Context Switch
   * We save current state and load the next state.
   */
  __asm__ volatile(
      "pushl $1f      \n\t" // Push resumption address
      "pushf          \n\t" // Save current flags
      "pusha          \n\t" // Save all registers
      "movl %%esp, %0 \n\t" // Store current ESP
      "movl %1, %%esp \n\t" // Load next ESP
      "popa           \n\t"
      "popf           \n\t" // Restores flags
      "ret            \n\t" // Jump to resumption point OR process entry
      "1:             \n\t" // Resumption point
      : "=m"(proc_table[prev_pid].stackptr)
      : "m"(proc_table[next_pid].stackptr)
      : "memory");
}
