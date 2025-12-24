global timer_stub
extern timer_handler

section .text
timer_stub:
    pusha                   ; Save all general-purpose registers
    call timer_handler      ; Call the C handler
    popa                    ; Restore all registers
    iret                    ; Return from interrupt