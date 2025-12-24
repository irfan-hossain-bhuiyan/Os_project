/*
 * context_switch.s - Context switching for kacchiOS
 * 
 * void context_switch(uintptr_t **old_sp, uintptr_t **new_sp)
 * 
 * This function saves the current CPU state (registers and flags) to the
 * old process's stack, then switches to the new process's stack and 
 * restores its CPU state.
 * 
 * Arguments:
 *   old_sp - Pointer to where we should save the current stack pointer
 *   new_sp - Pointer to where we should load the new stack pointer from
 * 
 * Stack layout on entry (from C call):
 *   [Return Address] <- ESP points here initially
 *   [old_sp]         <- First argument at ESP+4 (before any pushes)
 *   [new_sp]         <- Second argument at ESP+8 (before any pushes)
 * 
 * After saving state:
 *   [Return Address]
 *   [old_sp]
 *   [new_sp]
 *   [EFLAGS]         <- 4 bytes
 *   [EDI]            <- 4 bytes
 *   [ESI]            <- 4 bytes  
 *   [EBP]            <- 4 bytes
 *   [ESP]            <- 4 bytes (ignored by popa)
 *   [EBX]            <- 4 bytes
 *   [EDX]            <- 4 bytes
 *   [ECX]            <- 4 bytes
 *   [EAX]            <- 4 bytes, ESP points here
 * 
 * Total pushed: 36 bytes (4 for EFLAGS + 32 for 8 registers)
 * So old_sp is now at offset 36+4 = 40 from current ESP
 * And new_sp is at offset 36+8 = 44 from current ESP
 */

.global context_switch
.type context_switch, @function

context_switch:
    /* 1. Save the current task's CPU state */
    pushf                    # Push EFLAGS (flags register) - 4 bytes
    pusha                    # Push all general purpose registers - 32 bytes
                            # Order: EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI

    /* 2. Save current stack pointer to *old_sp */
    mov 40(%esp), %eax      # Load address of old_sp (first argument)
                            # 40 = 36 bytes pushed + 4 bytes return address
    mov %esp, (%eax)        # Store current ESP at the location pointed by old_sp
                            # Now *old_sp contains the current stack pointer

    /* 3. Load new stack pointer from *new_sp */
    mov 44(%esp), %eax      # Load address of new_sp (second argument)
                            # 44 = 36 bytes pushed + 8 bytes (return addr + first arg)
    mov (%eax), %esp        # Load the value at *new_sp into ESP
                            # Now ESP points to the new task's saved state

    /* 4. Restore the new task's CPU state */
    popa                    # Pop all general purpose registers (32 bytes)
                            # Order: EDI, ESI, EBP, ESP(ignored), EBX, EDX, ECX, EAX
    popf                    # Pop EFLAGS (4 bytes)

    /* 5. Return to the new task */
    ret                     # Pop return address and jump to it
                            # For a new task, this jumps to the entry function
                            # For a switched task, returns to where it was paused
