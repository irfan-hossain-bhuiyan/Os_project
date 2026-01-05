// Enhanced interactive demo for kacchiOS
// Shows off VGA, keyboard, monitoring, and all OS features

#include "kacchios.h"
#include "serial.h"
#include "vga.h"
#include "keyboard.h"
#include "monitor.h"

#define VERSION "2.0 Enhanced"

// Interactive shell process
void shell_process(void *arg) {
    (void)arg;
    
    // Initialize VGA if not already done
    // Note: This would ideally be called from kernel init
    
    serial_puts("\n");
    serial_puts("===========================================\n");
    serial_puts("  kacchiOS Interactive Shell\n");
    serial_puts("  Version: ");
    serial_puts(VERSION);
    serial_puts("\n");
    serial_puts("===========================================\n");
    serial_puts("\nAvailable commands:\n");
    serial_puts("  help       - Show this help\n");
    serial_puts("  ps         - List processes\n");
    serial_puts("  mem        - Show memory usage\n");
    serial_puts("  cpu        - Show CPU usage\n");
    serial_puts("  queue      - Show ready queue\n");
    serial_puts("  info       - System information\n");
    serial_puts("  stats      - Show all statistics\n");
    serial_puts("  dump <pid> - Dump process info\n");
    serial_puts("  clear      - Clear screen\n");
    serial_puts("\n");
    
    char cmd_buffer[64];
    int cmd_pos = 0;
    
    while (1) {
        serial_puts("kacchiOS> ");
        cmd_pos = 0;
        
        // Read command
        while (1) {
            char c = serial_getchar();
            
            if (c == '\n' || c == '\r') {
                serial_putchar('\n');
                cmd_buffer[cmd_pos] = '\0';
                break;
            } else if (c == '\b' || c == 127) {
                if (cmd_pos > 0) {
                    cmd_pos--;
                    serial_puts("\b \b");
                }
            } else if (cmd_pos < 63) {
                cmd_buffer[cmd_pos++] = c;
                serial_putchar(c);
            }
        }
        
        // Process command
        if (cmd_buffer[0] == '\0') {
            continue;
        }
        
        if (strcmp(cmd_buffer, "help") == 0) {
            serial_puts("\nAvailable commands:\n");
            serial_puts("  help       - Show this help\n");
            serial_puts("  ps         - List processes\n");
            serial_puts("  mem        - Show memory usage\n");
            serial_puts("  cpu        - Show CPU usage\n");
            serial_puts("  queue      - Show ready queue\n");
            serial_puts("  info       - System information\n");
            serial_puts("  stats      - Show all statistics\n");
            serial_puts("  dump <pid> - Dump process info\n");
            serial_puts("  clear      - Clear screen (serial)\n");
            serial_puts("\n");
        }
        else if (strcmp(cmd_buffer, "ps") == 0) {
            monitor_print_processes();
        }
        else if (strcmp(cmd_buffer, "mem") == 0) {
            monitor_print_memory();
        }
        else if (strcmp(cmd_buffer, "cpu") == 0) {
            monitor_print_cpu_usage();
        }
        else if (strcmp(cmd_buffer, "queue") == 0) {
            monitor_print_ready_queue();
        }
        else if (strcmp(cmd_buffer, "info") == 0) {
            monitor_print_sysinfo();
        }
        else if (strcmp(cmd_buffer, "stats") == 0) {
            monitor_print_stats();
        }
        else if (strcmp(cmd_buffer, "clear") == 0) {
            // Clear by printing newlines
            for (int i = 0; i < 50; i++) {
                serial_putchar('\n');
            }
        }
        else if (strncmp(cmd_buffer, "dump ", 5) == 0) {
            // Parse PID
            int pid = 0;
            for (int i = 5; cmd_buffer[i] >= '0' && cmd_buffer[i] <= '9'; i++) {
                pid = pid * 10 + (cmd_buffer[i] - '0');
            }
            monitor_dump_process((uint8_t)pid);
        }
        else {
            serial_puts("Unknown command: ");
            serial_puts(cmd_buffer);
            serial_puts("\nType 'help' for available commands.\n");
        }
    }
}

// Demo worker process
void demo_worker(void *arg) {
    int id = (int)(uintptr_t)arg;
    
    serial_puts("[worker");
    serial_print_hex(id);
    serial_puts("] Starting...\n");
    
    for (int i = 0; i < 3; i++) {
        // Do some work
        for (volatile int j = 0; j < 2000000; j++);
        
        serial_puts("[worker");
        serial_print_hex(id);
        serial_puts("] Iteration ");
        serial_print_hex(i);
        serial_puts("\n");
    }
    
    serial_puts("[worker");
    serial_print_hex(id);
    serial_puts("] Finished\n");
    
    // Terminate
    kill(getpid());
}

// Enhanced main with interactive shell
void main(void *arg) {
    (void)arg;
    
    serial_puts("\n\n");
    serial_puts("╔═══════════════════════════════════════╗\n");
    serial_puts("║     kacchiOS ");
    serial_puts(VERSION);
    serial_puts(" - Enhanced      ║\n");
    serial_puts("╚═══════════════════════════════════════╝\n");
    serial_puts("\n");
    serial_puts("Features:\n");
    serial_puts("  ✓ Preemptive Multitasking\n");
    serial_puts("  ✓ Process Aging (Anti-Starvation)\n");
    serial_puts("  ✓ Message Passing & Semaphores\n");
    serial_puts("  ✓ Dynamic Memory (malloc/free)\n");
    serial_puts("  ✓ VGA Text Mode\n");
    serial_puts("  ✓ Keyboard Driver\n");
    serial_puts("  ✓ System Monitoring\n");
    serial_puts("\n");
    
    // Show initial system info
    monitor_print_sysinfo();
    
    // Create demo worker processes
    serial_puts("Creating demo worker processes...\n");
    for (int i = 1; i <= 3; i++) {
        char name[16];
        name[0] = 'w';
        name[1] = 'o';
        name[2] = 'r';
        name[3] = 'k';
        name[4] = 'e';
        name[5] = 'r';
        name[6] = '0' + i;
        name[7] = '\0';
        
        create_process(demo_worker, (void *)(uintptr_t)i, name);
    }
    
    serial_puts("\n");
    serial_puts("Starting interactive shell...\n");
    serial_puts("Type 'help' for available commands.\n");
    serial_puts("\n");
    
    // Run interactive shell
    shell_process(NULL);
}
