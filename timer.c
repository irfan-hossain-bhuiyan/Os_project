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

void timer_handler(void) {
    ticks++;
    if (ticks % 100 == 0) {
        // Switch between PID 0 (Process A) and PID 1 (Process B)
        uint8_t next = (current_pid == 0) ? 1 : 0;
        switch_process(next);
    }
    pic_send_eoi(0); // IRQ0
}

extern void timer_stub();

void timer_init(void) {
    ticks = 0;
    pit_init(100); // Set timer to 100 Hz
    idt_set_gate(32, (uint32_t)timer_stub, 0x08, 0x8E); // Map IRQ0 to timer_stub
    pic_remap(); // Remap PIC interrupts
    asm volatile ("sti"); // Enable interrupts
}
