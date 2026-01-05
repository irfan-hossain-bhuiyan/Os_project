#include "monitor.h"
#include "process.h"
#include "serial.h"
#include "timer.h"
#include "heap.h"
#include "stack.h"

// Helper to print state as string
static const char *state_to_string(uint8_t state)
{
    switch (state)
    {
    case PROC_FREE:
        return "FREE";
    case PROC_CURRENT:
        return "CURRENT";
    case PROC_READY:
        return "READY";
    case PROC_WAITING:
        return "WAITING";
    case PROC_RECV:
        return "RECV";
    case PROC_TERMINATED:
        return "TERMINATED";
    default:
        return "UNKNOWN";
    }
}

void monitor_print_sysinfo(void)
{
    serial_puts("\n");
    serial_puts("==========================================\n");
    serial_puts("         kacchiOS System Information\n");
    serial_puts("==========================================\n");
    serial_puts("OS Name:        kacchiOS\n");
    serial_puts("Version:        2.0 (Enhanced)\n");
    serial_puts("Architecture:   x86 (i386)\n");
    serial_puts("Max Processes:  254\n");
    serial_puts("Stack Size:     4KB per process\n");
    serial_puts("Time Quantum:   20ms\n");
    serial_puts("Scheduler:      Round-Robin with Aging\n");
    serial_puts("==========================================\n\n");
}

void monitor_print_processes(void)
{
    serial_puts("\n");
    serial_puts("==========================================\n");
    serial_puts("           Process Table\n");
    serial_puts("==========================================\n");
    serial_puts("PID | State      | Name           | Age | Ticks\n");
    serial_puts("----+------------+----------------+-----+-------\n");

    int count = 0;
    for (int i = 0; i < NPROC; i++)
    {
        if (proc_table[i].state != PROC_FREE)
        {
            serial_print_hex(i);
            serial_puts("  | ");

            const char *state = state_to_string(proc_table[i].state);
            serial_puts(state);

            // Pad state to 10 chars
            int pad = 10 - strlen(state);
            for (int j = 0; j < pad; j++)
            {
                serial_putchar(' ');
            }
            serial_puts(" | ");

            // Print name (pad to 14 chars)
            char name[16];
            strcpy(name, proc_table[i].name);
            int name_len = strlen(name);
            serial_puts(name);
            for (int j = name_len; j < 14; j++)
            {
                serial_putchar(' ');
            }
            serial_puts(" | ");

            // Print age
            serial_print_hex(proc_table[i].age);
            serial_puts(" | ");

            // Print total ticks
            serial_print_hex(proc_table[i].total_ticks);
            serial_puts("\n");

            count++;
        }
    }

    serial_puts("----+------------+----------------+-----+-------\n");
    serial_puts("Total processes: ");
    serial_print_hex(count);
    serial_puts("\n\n");
}

void monitor_print_ready_queue(void)
{
    serial_puts("\n");
    serial_puts("==========================================\n");
    serial_puts("           Ready Queue\n");
    serial_puts("==========================================\n");

    if (ready_list == 255)
    {
        serial_puts("Ready queue is empty.\n\n");
        return;
    }

    serial_puts("Current PID: ");
    serial_print_hex(current_pid);
    serial_puts("\n\n");

    serial_puts("Ready Queue (circular):\n");
    pidtype curr = ready_list;
    pidtype start = ready_list;
    int pos = 0;

    do
    {
        serial_puts("  [");
        serial_print_hex(pos++);
        serial_puts("] PID ");
        serial_print_hex(curr);
        serial_puts(" (");
        serial_puts(proc_table[curr].name);
        serial_puts(") - ");
        serial_puts(state_to_string(proc_table[curr].state));

        if (curr == current_pid)
        {
            serial_puts(" <-- CURRENT");
        }

        serial_puts("\n");

        curr = get_next_node(curr);

        if (pos > 50)
        { // Safety: prevent infinite loop in case of corruption
            serial_puts("  ... (truncated, possible corruption)\n");
            break;
        }
    } while (curr != start);

    serial_puts("\n");
}

void monitor_print_cpu_usage(void)
{
    serial_puts("\n");
    serial_puts("==========================================\n");
    serial_puts("         CPU Usage Statistics\n");
    serial_puts("==========================================\n");

    // Calculate total ticks
    uint32_t total_ticks = 0;
    for (int i = 0; i < NPROC; i++)
    {
        if (proc_table[i].state != PROC_FREE)
        {
            total_ticks += proc_table[i].total_ticks;
        }
    }

    if (total_ticks == 0)
    {
        serial_puts("No CPU time recorded yet.\n\n");
        return;
    }

    serial_puts("PID | Name           | Ticks  | CPU %\n");
    serial_puts("----+----------------+--------+-------\n");

    for (int i = 0; i < NPROC; i++)
    {
        if (proc_table[i].state != PROC_FREE && proc_table[i].total_ticks > 0)
        {
            serial_print_hex(i);
            serial_puts("  | ");

            char name[16];
            strcpy(name, proc_table[i].name);
            int name_len = strlen(name);
            serial_puts(name);
            for (int j = name_len; j < 14; j++)
            {
                serial_putchar(' ');
            }
            serial_puts(" | ");

            serial_print_hex(proc_table[i].total_ticks);
            serial_puts("   | ");

            // Calculate percentage (scaled by 100 to avoid floating point)
            uint32_t percentage = (proc_table[i].total_ticks * 100) / total_ticks;
            serial_print_hex(percentage);
            serial_puts("%\n");
        }
    }

    serial_puts("----+----------------+--------+-------\n");
    serial_puts("Total ticks: ");
    serial_print_hex(total_ticks);
    serial_puts("\n\n");
}

void monitor_print_memory(void)
{
    serial_puts("\n");
    serial_puts("==========================================\n");
    serial_puts("         Memory Usage\n");
    serial_puts("==========================================\n");

    // Get heap statistics
    size_t heap_used, heap_free, heap_total;
    heap_stats(&heap_used, &heap_free, &heap_total);

    serial_puts("Heap Usage:\n");
    serial_puts("  Total:      ");
    serial_print_hex(heap_total);
    serial_puts(" bytes\n");
    serial_puts("  Used:       ");
    serial_print_hex(heap_used);
    serial_puts(" bytes\n");
    serial_puts("  Free:       ");
    serial_print_hex(heap_free);
    serial_puts(" bytes\n");
    serial_puts("  Utilization: ");
    if (heap_total > 0)
    {
        uint32_t percent = (heap_used * 100) / heap_total;
        serial_print_hex(percent);
        serial_puts("%\n");
    }
    else
    {
        serial_puts("N/A\n");
    }

    // Get stack usage
    int stacks_used = get_stack_usage();
    serial_puts("\nStack Usage:\n");
    serial_puts("  Stacks allocated: ");
    serial_print_hex(stacks_used);
    serial_puts("\n");
    serial_puts("  Memory used:      ");
    serial_print_hex(stacks_used * 4096);
    serial_puts(" bytes\n");

    serial_puts("\n");
}

void monitor_dump_process(uint8_t pid)
{
    if (pid >= NPROC)
    {
        serial_puts("Invalid PID\n");
        return;
    }

    if (proc_table[pid].state == PROC_FREE)
    {
        serial_puts("Process slot is free\n");
        return;
    }

    serial_puts("\n");
    serial_puts("==========================================\n");
    serial_puts("      Process Information Dump\n");
    serial_puts("==========================================\n");

    serial_puts("PID:          ");
    serial_print_hex(pid);
    serial_puts("\n");

    serial_puts("Name:         ");
    serial_puts(proc_table[pid].name);
    serial_puts("\n");

    serial_puts("State:        ");
    serial_puts(state_to_string(proc_table[pid].state));
    serial_puts("\n");

    serial_puts("Stack Base:   ");
    serial_print_hex((uintptr_t)proc_table[pid].stackbase);
    serial_puts("\n");

    serial_puts("Stack Ptr:    ");
    serial_print_hex((uintptr_t)proc_table[pid].stackptr);
    serial_puts("\n");

    serial_puts("Age:          ");
    serial_print_hex(proc_table[pid].age);
    serial_puts(" ticks\n");

    serial_puts("Total Ticks:  ");
    serial_print_hex(proc_table[pid].total_ticks);
    serial_puts("\n");

    serial_puts("Has Message:  ");
    serial_puts(proc_table[pid].has_message ? "Yes" : "No");
    serial_puts("\n");

    if (proc_table[pid].has_message)
    {
        serial_puts("Message:      ");
        serial_print_hex(proc_table[pid].msg);
        serial_puts("\n");
    }

    serial_puts("\n");
}

void monitor_memory_dump(void *addr, size_t len)
{
    uint8_t *ptr = (uint8_t *)addr;

    serial_puts("\nMemory Dump at ");
    serial_print_hex((uintptr_t)addr);
    serial_puts(" (");
    serial_print_hex(len);
    serial_puts(" bytes):\n\n");

    for (size_t i = 0; i < len; i += 16)
    {
        // Print address
        serial_print_hex((uintptr_t)(ptr + i));
        serial_puts(": ");

        // Print hex bytes
        for (size_t j = 0; j < 16 && (i + j) < len; j++)
        {
            uint8_t byte = ptr[i + j];
            const char hex[] = "0123456789ABCDEF";
            serial_putchar(hex[(byte >> 4) & 0xF]);
            serial_putchar(hex[byte & 0xF]);
            serial_putchar(' ');
        }

        // Pad if less than 16 bytes
        for (size_t j = i + 16; j < len && j < i + 16; j++)
        {
            serial_puts("   ");
        }

        serial_puts(" | ");

        // Print ASCII
        for (size_t j = 0; j < 16 && (i + j) < len; j++)
        {
            uint8_t byte = ptr[i + j];
            if (byte >= 32 && byte <= 126)
            {
                serial_putchar(byte);
            }
            else
            {
                serial_putchar('.');
            }
        }

        serial_puts("\n");
    }

    serial_puts("\n");
}

void monitor_print_stats(void)
{
    monitor_print_sysinfo();
    monitor_print_processes();
    monitor_print_ready_queue();
    monitor_print_cpu_usage();
    monitor_print_memory();
}
