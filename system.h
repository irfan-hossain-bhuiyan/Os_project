#ifndef SYSTEM_H
#define SYSTEM_H

#include "types.h"

// Terminates the system/QEMU using the isa-debug-exit device.
// status: Exit status code.
void system_terminate(uint32_t status);

#endif // SYSTEM_H
