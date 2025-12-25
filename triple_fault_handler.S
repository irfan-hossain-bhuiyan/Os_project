#include "serial.h"

.section .text
.global triple_fault_handler
triple_fault_handler:
    cli
    call serial_puts
    .asciz "[TRIPLE FAULT] System Halted\n"
1:
    hlt
    jmp 1b