    .global timer_stub
    .extern timer_handler

    .section .text
    timer_stub:
        pusha                   # Push all general-purpose registers
        call timer_handler      # Call the C handler
        popa                    # Pop all general-purpose registers
        iret                    # Return from interrupt
