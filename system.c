#include "system.h"
#include "io.h"

void system_terminate(uint32_t status) {
    // QEMU isa-debug-exit uses port 0x501. 
    // The exit status is (val << 1) | 1. 
    // We send the 'status' to this port to quit QEMU.
    uint16_t port = 0x501;
    __asm__ volatile ("outl %0, %1" : : "a"(status), "d"(port));
    
    // Fallback infinite loop if outl fails or not running in QEMU
    while (1) {
        __asm__ volatile ("hlt");
    }
}
