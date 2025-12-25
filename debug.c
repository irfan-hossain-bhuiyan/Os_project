#include "debug.h"
#include "serial.h"

void klog_error(const char* msg) {
#ifdef KERNEL_DEBUG
    serial_puts("[ERROR] ");
    serial_puts(msg);
    serial_puts("\n");
#else
    // In a hosted environment, we could use printf if we wanted,
    // but for now, we'll keep it silent or use a generic approach.
    (void)msg;
#endif
}
