#ifndef DEBUG_H
#define DEBUG_H

#include "types.h"

// If KERNEL_DEBUG is defined, we enable serial-based debugging.
// This is typically defined in the Makefile for kernel builds.

#ifdef KERNEL_DEBUG
    #include "serial.h"
    #define kdebug_puts(s)    serial_puts(s)
    #define kdebug_puthex(v)  serial_print_hex(v)
#else
    #define kdebug_puts(s)    ((void)0)
    #define kdebug_puthex(v)  ((void)0)
#endif

// Log an error message (typically always enabled in development)
void klog_error(const char* msg);

#endif // DEBUG_H
