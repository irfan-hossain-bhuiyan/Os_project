/*
 * void context_switch(uint32_t **old_sp, uint32_t *new_sp)
 * - Saves current context, stores ESP to *old_sp
 * - Loads ESP from new_sp, restores context, resumes
 * - C prototype: extern void context_switch(uint32_t **old_sp, uint32_t *new_sp);
 */
.global context_switch
.type context_switch, @function

context_switch:
    // 1. Save current task's state
    pushf              # Push EFLAGS (4 bytes)
    pusha              # Push EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI (32 bytes)

    // Now stack has: [Old EIP][old_sp][new_sp]...[EFLAGS][Pushed Regs]
    // Total pushed: 36 bytes + 4 bytes (EIP) = 40 bytes.
    // old_sp (Arg 1) is at 40(%esp)
    // new_sp (Arg 2) is at 44(%esp)

    // 2. Save current ESP into *old_sp
    mov 40(%esp), %eax   # Get pointer to old_sp
    mov %esp, (%eax)     # *old_sp = current ESP

    // 3. Load new ESP from new_sp
    mov 44(%esp), %eax   # Get new_sp value (this is the pointer to the new stack)
    mov (%eax), %esp     # ESP = *new_sp (load the value pointed to by new_sp)

    // 4. Restore new task's state
    popa
    popf
    ret

