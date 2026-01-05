#ifndef MONITOR_H
#define MONITOR_H

#include "types.h"

// System monitoring and statistics

// Get system statistics
void monitor_print_stats(void);

// Print process table
void monitor_print_processes(void);

// Print memory usage
void monitor_print_memory(void);

// Print CPU usage per process
void monitor_print_cpu_usage(void);

// Print system information
void monitor_print_sysinfo(void);

// Dump process information for specific PID
void monitor_dump_process(uint8_t pid);

// Show ready queue
void monitor_print_ready_queue(void);

// Memory dump (hex + ASCII)
void monitor_memory_dump(void *addr, size_t len);

#endif // MONITOR_H
