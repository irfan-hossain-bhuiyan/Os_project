// main3.c - Test for race condition and process context switching
#include "kacchios.h"

// Global variable that multiple processes will mutate
volatile uint32_t shared_counter = 0;

// Semaphore for synchronization (main waits for all processes to finish)
int done_sem;

// Worker function - increments shared counter 2000 times
void worker(void *arg) {
  (void)arg;
  // Increment shared counter 2000 times (race condition!)
  for (int i = 0; i < 1000; i++) {
    uint32_t updated_counter = shared_counter + 1;
    for (volatile int i = 0; i < 10000; i++) {
    };
    shared_counter = updated_counter;
  }

  // Signal that this process is done
  sem_signal(done_sem);
}

// Main function to test race conditions
void main(void) {
  serial_puts("\n=== Race Condition Test ===\n");

  // Create semaphore (initial count = 0, main will wait 5 times)
  done_sem = sem_create(0);
  if (done_sem < 0) {
    serial_puts("[ERROR] Failed to create semaphore\n");
    while (1) {
      __asm__("hlt");
    }
  }

  // Create 5 processes that will compete for shared_counter
  serial_puts("Creating 5 processes...\n");
  for (int i = 0; i < 5; i++) {
    pidtype pid = create_process(worker, 0, "Worker");
    if (pid == 255) {
      serial_puts("[ERROR] Failed to create process\n");
      while (1) {
        __asm__("hlt");
      }
    }
  }

  // Wait for all 5 processes to finish
  serial_puts("Waiting for processes to complete...\n");
  for (int i = 0; i < 5; i++) {
    sem_wait(done_sem);
  }

  // Print results
  serial_puts("\n=== Results ===\n");
  serial_puts("Expected: 5000\n");
  serial_puts("Got: ");

  // Convert shared_counter to string
  char buf[16];
  uint32_t val = shared_counter;
  int idx = 0;
  if (val == 0) {
    buf[idx++] = '0';
  } else {
    char temp[16];
    int j = 0;
    while (val > 0) {
      temp[j++] = '0' + (val % 10);
      val /= 10;
    }
    while (j > 0) {
      buf[idx++] = temp[--j];
    }
  }
  buf[idx] = '\0';
  serial_puts(buf);
  serial_puts("\n");

  if (shared_counter == 5000) {
    serial_puts("[PASS] No race condition detected\n");
  } else {
    serial_puts("[FAIL] Race condition! Lost updates detected\n");
  }

  serial_puts("\nTest complete.\n");
  while (1) {
    __asm__("hlt");
  }
}
