#include "serial.h"
#include "io.h"
#include "idt.h"
#include "pic.h"

#define PIT_CONTROL 0x43
#define PIT_COUNTER0 0x40
#define PIT_FREQUENCY 1193182

static uint32_t ticks = 0;

static void pit_init(uint32_t frequency) {
    uint32_t divisor = PIT_FREQUENCY / frequency;
    outb(PIT_CONTROL, 0x36); // Set PIT to mode 3 (square wave generator)
    outb(PIT_COUNTER0, divisor & 0xFF); // Low byte
    outb(PIT_COUNTER0, (divisor >> 8) & 0xFF); // High byte
}

#include "process.h"

// 1000 Hz = 1ms period
// Trigger reshed every 16 ticks (~16ms time slice)
void timer_handler(void) {
    ticks++;
    
    // Send EOI to PIC
    pic_send_eoi(0);

    if ((ticks & 0xF) == 0 && current_pid != 255) { // ticks % 16 == 0
        reshed();
    }
}

extern void timer_stub();

void timer_init(void) {
    ticks = 0;
    pit_init(1000); // Set timer to 1000 Hz (1ms)
    idt_set_gate(32, (uint32_t)timer_stub, 0x08, 0x8E); // Map IRQ0 to timer_stub
    pic_remap(); // Remap PIC interrupts
    asm volatile ("sti"); // Enable interrupts
}