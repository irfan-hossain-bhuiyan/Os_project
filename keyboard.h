#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

// Keyboard scan code sets
#define KB_DATA_PORT 0x60
#define KB_STATUS_PORT 0x64
#define KB_COMMAND_PORT 0x64

// Special keys
#define KEY_ESC 0x01
#define KEY_BACKSPACE 0x0E
#define KEY_TAB 0x0F
#define KEY_ENTER 0x1C
#define KEY_LCTRL 0x1D
#define KEY_LSHIFT 0x2A
#define KEY_RSHIFT 0x36
#define KEY_LALT 0x38
#define KEY_CAPS 0x3A
#define KEY_F1 0x3B
#define KEY_F2 0x3C
#define KEY_F3 0x3D
#define KEY_F4 0x3E
#define KEY_F5 0x3F
#define KEY_F6 0x40
#define KEY_F7 0x41
#define KEY_F8 0x42
#define KEY_F9 0x43
#define KEY_F10 0x44
#define KEY_NUMLOCK 0x45
#define KEY_SCROLLLOCK 0x46

// Keyboard buffer size
#define KB_BUFFER_SIZE 128

// Initialize keyboard driver
void keyboard_init(void);

// Check if key is available
int keyboard_available(void);

// Read a character from keyboard (blocking)
char keyboard_getchar(void);

// Read a character from keyboard (non-blocking, returns 0 if none)
char keyboard_trygetchar(void);

// Get raw scancode (for advanced use)
uint8_t keyboard_get_scancode(void);

#endif // KEYBOARD_H
