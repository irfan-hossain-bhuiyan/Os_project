#include "types.h"
#include "io.h"
#include "serial.h"
#include "system.h"

#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8E

// Structure representing an entry in the Interrupt Descriptor Table (IDT)
struct idt_entry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idt_entry idt[IDT_SIZE];
struct idt_ptr idtp;

// Loads the address of the IDT into the CPU's IDT register (IDTR) using inline assembly.
// This enables the CPU to use the new IDT for interrupt handling.
static inline void load_idt(uint32_t idtp_addr) {
    __asm__ volatile ("lidt (%0)" : : "r"(idtp_addr));
}

// Sets an entry in the IDT for a given interrupt vector.
// num: Interrupt vector number (0-255)
// base: Address of the handler function
// sel: Code segment selector in GDT
// flags: Type and attributes (e.g., present, DPL, gate type)
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

// Triple fault handler: Prints a message and halts the CPU in an infinite loop.
void triple_fault_handler(void) {
    serial_puts("\n[IDT] TRIPLE FAULT: System Halted\n");
    system_terminate(42);
}

// Installs the IDT by initializing all entries and loading it into the CPU.
// Also sets the triple fault handler at vector 8 (Double Fault).
void idt_install() {
    idtp.limit = (sizeof(struct idt_entry) * IDT_SIZE) - 1;
    idtp.base = (uint32_t)&idt;

    for (int i = 0; i < IDT_SIZE; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // Set the triple fault handler (actually double fault, vector 8)
    idt_set_gate(8, (uint32_t)triple_fault_handler, 0x08, INTERRUPT_GATE);
    load_idt((uint32_t)&idtp);
}
