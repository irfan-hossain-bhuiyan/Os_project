#include "serial.h"
#include "io.h"
#include "idt.h"
#include "pic.h"

#define PIT_CONTROL 0x43
#define PIT_COUNTER0 0x40
#define PIT_FREQUENCY 1193182

static uint32_t ticks = 0;
static uint32_t time_slice = 2; // Default: 2 ticks = 20ms quantum

static void pit_init(uint32_t frequency) {
    uint32_t divisor = PIT_FREQUENCY / frequency;
    outb(PIT_CONTROL, 0x36); // Set PIT to mode 3 (square wave generator)
    outb(PIT_COUNTER0, divisor & 0xFF); // Low byte
    outb(PIT_COUNTER0, (divisor >> 8) & 0xFF); // High byte
}

#include "process.h"

// 100 Hz = 10ms period (standard for many Unix/Linux systems)
// Trigger reshed based on configurable time_slice quantum
void timer_handler(void) {
    ticks++;
    
    // Send EOI to PIC
    pic_send_eoi(0);

    if ((ticks % time_slice) == 0 && current_pid != 255) {
        reshed();
    }
}

extern void timer_stub();

void timer_init(void) {
    ticks = 0;
    pit_init(100); // Set timer to 100 Hz (10ms) - standard for Unix/Linux
    idt_set_gate(32, (uint32_t)timer_stub, 0x08, 0x8E); // Map IRQ0 to timer_stub
    pic_remap(); // Remap PIC interrupts
    asm volatile ("sti"); // Enable interrupts
}

// Function to set the time slice (quantum) in milliseconds
// Example: set_time_slice(10) = 10ms, set_time_slice(20) = 20ms, set_time_slice(100) = 100ms
// Note: Values will be rounded to nearest 10ms (tick resolution)
void set_time_slice(uint32_t milliseconds) {
    if (milliseconds > 0) {
        // Convert milliseconds to ticks (10ms per tick at 100 Hz)
        time_slice = (milliseconds + 5) / 10; // Round to nearest tick
        if (time_slice == 0) {
            time_slice = 1; // Minimum 1 tick (10ms)
        }
    }
}

// Get current time slice setting in milliseconds
uint32_t get_time_slice(void) {
    return time_slice * 10; // Convert ticks to milliseconds
}
