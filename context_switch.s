/*
 * void context_switch(uint32_t **old_sp, uint32_t *new_sp)
 * - Saves current context, stores ESP to *old_sp
 * - Loads ESP from new_sp, restores context, resumes
 * - C prototype: extern void context_switch(uint32_t **old_sp, uint32_t *new_sp);
 */
.global context_switch
.type context_switch, @function

context_switch:
    // Save callee-saved registers and EFLAGS
    pusha              # Push EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
    pushf              # Push EFLAGS

    # Save current ESP to *old_sp
    mov  44(%esp), %eax  # Get old_sp argument (ESP + 44 after pusha/pushf)
    lea  12(%esp), %edx  # ESP value after pusha/pushf (where EFLAGS is)
    mov  %edx, (%eax)    # *old_sp = current ESP

    # Load new ESP from new_sp argument
    mov  48(%esp), %eax  # Load new_sp argument (ESP + 48 after pusha/pushf)
    mov  (%eax), %esp
    mov  (%eax), %esp

    // Restore EFLAGS and registers from new stack
    popf
    popa

    ret
