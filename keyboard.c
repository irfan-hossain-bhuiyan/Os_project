#include "keyboard.h"
#include "io.h"
#include "idt.h"
#include "pic.h"

// Keyboard state
static char kb_buffer[KB_BUFFER_SIZE];
static volatile int kb_read_pos = 0;
static volatile int kb_write_pos = 0;
static volatile int kb_count = 0;

// Modifier key states
static volatile int shift_pressed = 0;
static volatile int ctrl_pressed = 0;
static volatile int alt_pressed = 0;
static volatile int caps_lock = 0;

// US QWERTY keyboard layout (scan code to ASCII)
static const char scancode_to_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, // Control
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, // Left shift
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0, // Right shift
    '*',
    0, // Alt
    ' ', // Space
    0, // Caps lock
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F1-F10
    0, // Num lock
    0, // Scroll lock
    0, // Home
    0, // Up arrow
    0, // Page up
    '-',
    0, // Left arrow
    0,
    0, // Right arrow
    '+',
    0, // End
    0, // Down arrow
    0, // Page down
    0, // Insert
    0, // Delete
    0, 0, 0,
    0, 0 // F11, F12
};

static const char scancode_to_ascii_shift[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, // Control
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, // Left shift
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0, // Right shift
    '*',
    0, // Alt
    ' ', // Space
    0, // Caps lock
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F1-F10
    0, // Num lock
    0, // Scroll lock
    0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
    0, 0, 0,
    0, 0 // F11, F12
};

// Add character to buffer
static void kb_buffer_add(char c) {
    if (kb_count < KB_BUFFER_SIZE) {
        kb_buffer[kb_write_pos] = c;
        kb_write_pos = (kb_write_pos + 1) % KB_BUFFER_SIZE;
        kb_count++;
    }
}

// Keyboard interrupt handler
static void keyboard_handler(struct interrupt_frame *frame) {
    (void)frame;
    
    uint8_t scancode = inb(KB_DATA_PORT);
    
    // Check for key release (bit 7 set)
    if (scancode & 0x80) {
        // Key release
        scancode &= 0x7F;
        
        if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
            shift_pressed = 0;
        } else if (scancode == KEY_LCTRL) {
            ctrl_pressed = 0;
        } else if (scancode == KEY_LALT) {
            alt_pressed = 0;
        }
    } else {
        // Key press
        if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
            shift_pressed = 1;
        } else if (scancode == KEY_LCTRL) {
            ctrl_pressed = 1;
        } else if (scancode == KEY_LALT) {
            alt_pressed = 1;
        } else if (scancode == KEY_CAPS) {
            caps_lock = !caps_lock;
        } else {
            // Convert scancode to ASCII
            char c = 0;
            
            if (scancode < sizeof(scancode_to_ascii)) {
                if (shift_pressed) {
                    c = scancode_to_ascii_shift[scancode];
                } else {
                    c = scancode_to_ascii[scancode];
                    
                    // Apply caps lock for letters
                    if (caps_lock && c >= 'a' && c <= 'z') {
                        c = c - 'a' + 'A';
                    }
                }
                
                if (c != 0) {
                    kb_buffer_add(c);
                }
            }
        }
    }
    
    pic_send_eoi(1); // Keyboard is on IRQ1
}

void keyboard_init(void) {
    // Clear buffer
    kb_read_pos = 0;
    kb_write_pos = 0;
    kb_count = 0;
    
    // Register keyboard interrupt (IRQ1 = INT 33)
    idt_set_gate(33, (uint32_t)keyboard_handler, 0x08, 0x8E);
    
    // Enable keyboard interrupt
    pic_clear_mask(1);
}

int keyboard_available(void) {
    return kb_count > 0;
}

char keyboard_trygetchar(void) {
    if (kb_count == 0) {
        return 0;
    }
    
    __asm__ volatile("cli");
    char c = kb_buffer[kb_read_pos];
    kb_read_pos = (kb_read_pos + 1) % KB_BUFFER_SIZE;
    kb_count--;
    __asm__ volatile("sti");
    
    return c;
}

char keyboard_getchar(void) {
    // Busy wait until character available
    while (kb_count == 0) {
        __asm__ volatile("hlt"); // Wait for interrupt
    }
    
    return keyboard_trygetchar();
}

uint8_t keyboard_get_scancode(void) {
    // Wait for data available
    while (!(inb(KB_STATUS_PORT) & 0x01)) {
        __asm__ volatile("hlt");
    }
    
    return inb(KB_DATA_PORT);
}
