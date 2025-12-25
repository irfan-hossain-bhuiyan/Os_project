#include "serial.h"
#include <stdio.h>

void serial_init(void) {}
void serial_putc(char c) { putchar(c); }
void serial_puts(const char* str) { printf("%s", str); }
char serial_getc(void) { return 0; }
void serial_print_hex(uint32_t val) { printf("0x%08x", val); }
