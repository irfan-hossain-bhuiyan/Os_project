// Programmable Interrupt Controller (PIC) management for i386 systems.
// The PIC handles hardware interrupt requests (IRQs) and forwards them to the CPU.
// This code remaps the PIC to avoid conflicts with CPU exception vectors and manages interrupt masking.

#include "types.h"
#include "io.h"

#define PIC1 0x20      // IO base address for master PIC
#define PIC2 0xA0      // IO base address for slave PIC
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)
#define PIC_EOI 0x20   // End-of-interrupt command code

// Remaps the PIC interrupt vectors to avoid conflicts with CPU exceptions.
// After remapping, IRQs 0-7 are mapped to IDT vectors 0x20-0x27, and IRQs 8-15 to 0x28-0x2F.
// Also masks all interrupts except the timer (IRQ0).
void pic_remap() {
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    
    // Mask all interrupts except IRQ0 (Timer)
    outb(PIC1_DATA, 0xFE); // 11111110 (all masked except bit 0)
    outb(PIC2_DATA, 0xFF); // 11111111 (all masked)
}

// Sends an End-of-Interrupt (EOI) signal to the PIC after handling an IRQ.
// irq: The IRQ number that was handled (0-15).
// For IRQs >= 8, both slave and master PICs must be notified.
void pic_send_eoi(unsigned char irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}
