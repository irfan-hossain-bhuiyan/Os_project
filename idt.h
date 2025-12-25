#ifndef IDT_H
#define IDT_H
#include "types.h"

// Sets an entry in the Interrupt Descriptor Table (IDT)
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

// Installs and loads the IDT, enabling interrupt handling on i386
void idt_install();


#endif // IDT_H
