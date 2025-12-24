# Timer Mechanism in kacchiOS

This document explains how the timer works in kacchiOS. Even if you understand the **Interrupt Descriptor Table (IDT)** and general interrupt handling, there are several hardware components that must work together on the i386 architecture to make a timer "tick."

---

## 🏗️ Hardware Components

### 1. PIT (Programmable Interval Timer)
The **PIT** (Intel 8253/8254) is a chip that acts as the OS heartbeat.
*   **The Crystal:** It has an internal oscillator running at **1,193,182 Hz**.
*   **The Divisor:** Since 1.19 MHz is too fast for us to handle, we send the chip a "divisor." For example, if we want a **100 Hz** timer (100 ticks per second), we set the divisor to `1193182 / 100 ≈ 11931`.
*   **The Signal:** Every time the PIT counts down to zero, it pulses a physical wire called **IRQ0** (Interrupt Request 0).

### 2. PIC (Programmable Interrupt Controller)
The **PIC** (Intel 8259) is the "Traffic Cop" for hardware interrupts.
*   **Multiple Inputs:** It manages many wires (Timer, Keyboard, Mouse).
*   **Remapping:** By default, the PIC maps the timer to Interrupt 8. However, on x86, Interrupt 8 is reserved for CPU exceptions (Double Faults). We **remap** the PIC so that hardware interrupts start at index **32** in the IDT. This is why our timer is at `idt[32]`.
*   **EOI (End of Interrupt):** The PIC is protective. Once it sends an interrupt, it stops sending any more until the CPU explicitly says: *"I'm done, send the next one."* This is done by sending a specific byte (`0x20`) to the PIC's command port.

---

## 🔄 The Interrupt Flow

When the timer fires, the following sequence occurs:

1.  **Hardware Level:** The **PIT** pulses the wire → The **PIC** receives it and signals the **CPU**.
2.  **Assembly Entry (`timer_stub`):** 
    *   The CPU stops whatever it was doing (the "null process").
    *   It looks up entry **32** in the IDT and jumps to `timer_stub`.
    *   **Crucial Step:** Since C functions don't know they are in an interrupt, they might overwrite registers that the interrupted process was using. The stub uses `pusha` (Push All) to save the current state of the CPU.
3.  **C Logic (`timer_handler`):**
    *   The stub calls the C function.
    *   **Tick Counting:** We increment a `ticks` variable. Since our PIT is set to 100 Hz, we only print "..." when `ticks % 100 == 0`.
    *   **EOI:** We call `pic_send_eoi(0)` to tell the PIC it can resume sending interrupts.
4.  **Assembly Exit:**
    *   The handler returns to the stub.
    *   The stub calls `popa` to restore the saved registers.
    *   It executes `iret` (Interrupt Return), which tells the CPU to jump back to the exact instruction it was executing before the interrupt happened.

---

## 🛠️ Implementation Details

### Why 100 Hz?
While the goal is a 1-second interval, setting the hardware to 1 Hz is often less precise on old hardware/emulators. By running at 100 Hz, we create a higher resolution clock that we can use later for things like:
*   **Preemptive Scheduling:** Switching between processes every 10ms.
*   **Sleep functions:** Allowing a process to wait for exactly 50ms.

### The IDT Loading Fix
To make the CPU find our IDT, we use the `lidt` instruction. In assembly, we have to be careful:
```assembly
load_idt:
    mov 4(%esp), %eax  ; Get the pointer to the IDT structure from the stack
    lidt (%eax)        ; Tell the CPU where the table is
    ret
```
The first 4 bytes on the stack `(%esp)` is the return address of the function, so the actual pointer passed from C is at `4(%esp)`.
