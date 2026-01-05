#ifndef VGA_H
#define VGA_H

#include "types.h"

// VGA text mode constants
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

// VGA color codes
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

// Initialize VGA text mode
void vga_init(void);

// Clear the screen
void vga_clear(void);

// Set foreground and background colors
void vga_set_color(enum vga_color fg, enum vga_color bg);

// Put a character at the current cursor position
void vga_putchar(char c);

// Write a string
void vga_puts(const char *str);

// Write a string with length
void vga_write(const char *data, size_t size);

// Print hexadecimal value
void vga_print_hex(uint32_t value);

// Print decimal value
void vga_print_dec(uint32_t value);

// Set cursor position
void vga_set_cursor(uint8_t row, uint8_t col);

// Get cursor position
void vga_get_cursor(uint8_t *row, uint8_t *col);

// Scroll the screen up by one line
void vga_scroll(void);

// Enable/disable cursor
void vga_enable_cursor(void);
void vga_disable_cursor(void);

#endif // VGA_H
