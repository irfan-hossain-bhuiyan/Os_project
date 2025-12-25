#include "process.h"
#include "serial.h"
#include "stack.h"
#include "string.h"

void switch_process(uint8_t next_pid);

struct ProcessNode {
  pidtype before;
  pidtype after;
} ProcessNode;

struct Procent proc_table[NPROC];
struct ProcessNode proc_nodes[NPROC];

// Ready list head PID (255 means empty)
pidtype ready_list = 255;
pidtype proc_create(proc_entry_t entry, const void *arg, const char *name);

void node_append_before(pidtype pid, pidtype move_before) {
  pidtype before = proc_nodes[move_before].before;
  proc_nodes[pid].after = move_before;
  proc_nodes[pid].before = before;
  proc_nodes[before].after = pid;
  proc_nodes[move_before].before = pid;
}

void node_append_after(pidtype pid, pidtype move_after) {
  pidtype after = proc_nodes[move_after].after;
  proc_nodes[pid].before = move_after;
  proc_nodes[pid].after = after;
  proc_nodes[after].before = pid;
  proc_nodes[move_after].after = pid;
}

uint8_t current_pid = NPROC;
const size_t STACK_SIZE = 4096;

// Round-robin: switch to the next process in the ready list
void switch_to_next_process(void) {
  if (current_pid == 255 || ready_list == 255)
    return;
  pidtype next_pid = proc_nodes[current_pid].after;
  if (next_pid == 255)
    return;
  switch_process(next_pid);
}
void reshed(void) { switch_to_next_process(); }

void append_on_ready_list(pidtype pid) {
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

void null_process(void *arg) {
  (void)arg;
  reshed();
}
void init_proc_table() {
  for (int i = 0; i < NPROC; i++) {
    proc_table[i].state = PROC_FREE;
    proc_table[i].pid = i;
  }
}
void init_proc_nodes() {
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
  uint8_t pid = proc_create(entry, arg, name);
  if (pid != 255) {
    append_on_ready_list(pid);
  }
  return pid;
}

pidtype proc_create(proc_entry_t entry, const void *arg, const char *name) {
  /* Find a free slot */
  int pid = -1;
  for (int i = 0; i < NPROC; i++) {
    if (proc_table[i].state == PROC_FREE) {
      pid = i;
      break;
    }
  }

  if (pid == -1)
    return 255;

  void *stack = alloc_stack(STACK_SIZE);
  if (!stack)
    return 255;

  proc_table[pid].state = PROC_READY;
  proc_table[pid].stackbase = stack;

  /* Prepare initial stack frame */
  uintptr_t *sp = (uintptr_t *)((uint8_t *)stack + STACK_SIZE);

  *(--sp) = (uintptr_t)arg;   // Argument
  *(--sp) = 0;                // Dummy Return Address
  *(--sp) = (uintptr_t)entry; // Initial EIP
  *(--sp) = 0x002;            // EFLAGS (Interrupts disabled)

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
  /* Switch to PID 0 (the null process created in init_proc) */
  switch_process(0);
}

void switch_process(pidtype next_pid) {
  if (next_pid >= NPROC || proc_table[next_pid].state == PROC_FREE)
    return;

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

  proc_table[prev_pid].state = PROC_READY;
  proc_table[next_pid].state = PROC_CURRENT;

  /*
   * Inline Context Switch
   * We save current state and load the next state.
   */
  __asm__ volatile(
      "pushl $1f      \n\t" // Push resumption address
      "pushf          \n\t"
      "pusha          \n\t"
      "movl %%esp, %0 \n\t" // Store current ESP
      "movl %1, %%esp \n\t" // Load next ESP
      "popa           \n\t"
      "popf           \n\t"
      "ret            \n\t" // Jump to resumption point OR process entry
      "1:             \n\t" // Resumption point
      : "=m"(proc_table[prev_pid].stackptr)
      : "m"(proc_table[next_pid].stackptr)
      : "memory");
}
