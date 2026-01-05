#ifndef TIMER_H
#define TIMER_H

#include "types.h"

void timer_init(void);
void timer_handler(void);

// Configure the time slice (quantum) in milliseconds
// Example: set_time_slice(20) sets 20ms quantum
// Note: Values rounded to nearest 10ms (timer tick resolution at 100 Hz)
void set_time_slice(uint32_t milliseconds);

// Get current time slice setting in milliseconds
uint32_t get_time_slice(void);

#endif // TIMER_H
