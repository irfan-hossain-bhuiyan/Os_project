#include "types.h"
#include "serial.h"
#include "process.h"
#include "idt.h"
void process_a(void *arg) {
    (void)arg;
    int count = 0;
    while (1) {
        serial_puts("[A:");
        serial_print_hex(count++);
        serial_puts("] ");
        for (volatile int i = 0; i < 2000000; i++);
	reshed();
    }
}

void process_b(void *arg) {
    (void)arg;
    int count = 0;
    while (1) {
        serial_puts("[B:");
        serial_print_hex(count++);
        serial_puts("] ");
        for (volatile int i = 0; i < 2000000; i++);
	reshed();
    }
}

void process_c(void *arg) {
    (void)arg;
    int count = 0;
    while (1) {
        serial_puts("[C:");
        serial_print_hex(count++);
        serial_puts("] ");
        for (volatile int i = 0; i < 2000000; i++);
	reshed();
    }
}

extern pidtype ready_list;
extern pidtype get_next_node(pidtype pid);
extern pidtype get_previous_node(pidtype pid);
extern struct ProcessNode proc_nodes[NPROC];

void print_ready_list(void) {
    pidtype start = ready_list;
    pidtype pid = start;
    serial_puts("Ready list: ");
    for (int i = 0; i < 8; ++i) {
        serial_print_hex(pid);
        serial_puts(" ");
        pid = get_next_node(pid);
        if (pid == start) break;
    }
    serial_puts("\n");
}

void print_node_links(pidtype pid) {
    serial_puts("PID: ");
    serial_print_hex(pid);
    serial_puts(" Before: ");
    serial_print_hex(proc_nodes[pid].before);
    serial_puts(" After: ");
    serial_print_hex(proc_nodes[pid].after);
    serial_puts("\n");
}

void kmain(void) {
    serial_init();
    serial_puts("\n--- kacchiOS Multi-Process State Test ---\n");
    // Install the Interrupt Descriptor Table (IDT), including the triple fault handler at vector 8.
    idt_install();
    serial_puts("[DEBUG] IDT installed\n");
    init_proc(); // Creates Null Process at PID 0
    serial_puts("[DEBUG] Processes initialized\n");

    serial_puts("After init_proc (null process):\n");
    print_ready_list();
    print_node_links(0);

    serial_puts("Creating Process A (PID 1)...\n");
    create_process(process_a, NULL, "proc_a");
    print_ready_list();
    print_node_links(1);

    serial_puts("Creating Process B (PID 2)...\n");
    create_process(process_b, NULL, "proc_b");
    print_ready_list();
    print_node_links(2);

    serial_puts("Creating Process C (PID 3)...\n");
    create_process(process_c, NULL, "proc_c");
    print_ready_list();
    print_node_links(3);

    serial_puts("All node links after creation:\n");
    for (int i = 0; i < 4; ++i) {
        print_node_links(i);
    }

    serial_puts("Starting multitasking at A...\n");
    run_null_process();
    /* Should never reach here */
    while(1);
}

