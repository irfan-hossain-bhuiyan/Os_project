#include "kacchios.h"

// ============================================================
// HEAP ALLOCATION EXAMPLE - Prime Number Generator
// Uses malloc to allocate array of integers (not doubles)
// ============================================================

// Helper function to read an integer from serial input
int serial_read_int(void) {
    int num = 0;
    char c;
    
    while(1) {
        c = serial_getc();
        
        // Echo the character
        serial_putc(c);
        
        // Enter key pressed
        if(c == '\r' || c == '\n') {
            serial_puts("\n");
            break;
        }
        
        // If it's a digit, add to number
        if(c >= '0' && c <= '9') {
            num = num * 10 + (c - '0');
        }
    }
    
    return num;
}

void main(void) {
    serial_puts("\n\n");
    serial_puts("===========================================\n");
    serial_puts("  HEAP ALLOCATION EXAMPLE\n");
    serial_puts("  (Dynamic Prime Number Generator)\n");
    serial_puts("===========================================\n\n");
    
    // Get input from user
    serial_puts("How many primes to find? (Enter number): ");
    int n = serial_read_int();
    
    if(n <= 0 || n > 1000) {
        serial_puts("[ERROR] Invalid input! Must be between 1 and 1000.\n");
        return;
    }
    
    serial_puts("\n[Main] Finding first ");
    serial_print_hex(n);
    serial_puts(" prime numbers...\n\n");
    
    // Allocate array of INTEGERS (not doubles) using malloc
    int *primes = (int *)malloc(n * sizeof(int));
    
    if(primes == NULL) {
        serial_puts("[ERROR] malloc failed to allocate memory!\n");
        return;
    }
    
    serial_puts("[Main] Successfully allocated ");
    serial_print_hex(n * sizeof(int));
    serial_puts(" bytes for prime array\n\n");
    
    // Initialize first prime
    primes[0] = 2;
    int prime_count = 1;
    int candidate = 3;
    
    // Find primes
    while(prime_count < n) {
        int is_prime = 1;
        
        // Check if candidate is divisible by any existing prime
        for(int i = 0; i < prime_count; i++) {
            if(candidate % primes[i] == 0) {
                is_prime = 0;
                break;
            }
        }
        
        // If prime, add to array
        if(is_prime) {
            primes[prime_count] = candidate;
            prime_count++;
        }
        
        candidate += 2;  // Only check odd numbers
    }
    
    // Print all primes
    serial_puts("[Main] Prime numbers found:\n");
    for(int i = 0; i < n; i++) {
        serial_puts("  Prime[");
        serial_print_hex(i);
        serial_puts("] = ");
        serial_print_hex(primes[i]);
        serial_puts("\n");
    }
    
    serial_puts("\n[Main] Freeing allocated memory...\n");
    free(primes);
    
    serial_puts("[Main] Memory freed successfully!\n");
    serial_puts("===========================================\n\n");
}
