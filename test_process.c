#include "process.h"
#include <assert.h>
#include <stdio.h>

// Expose these for testing
extern pidtype ready_list;
extern pidtype get_next_node(pidtype pid);
extern pidtype get_previous_node(pidtype pid);
extern struct ProcessNode proc_nodes[NPROC];
void dummy_proc(void *arg) { (void)arg; }

int main() {
    // Initialize process system (creates null process at PID 0)
    init_proc();

    // Test: ready list should contain only the null process (PID 0)
    assert(ready_list == 0);
    assert(get_next_node(0) == 0);
    assert(get_previous_node(0) == 0);

    printf("[OK] Null process ready list is correct.\n");

    // Create 3 more processes
    pidtype p0 = 0;
    pidtype p1 = create_process(dummy_proc, NULL, "p1");
    pidtype p2 = create_process(dummy_proc, NULL, "p2");
    pidtype p3 = create_process(dummy_proc, NULL, "p3");

    // Check that all PIDs are valid
    assert(p1 != 255 && p2 != 255 && p3 != 255);

    // Traverse the ready list forward
pidtype start = ready_list;
pidtype n0 = start;
pidtype n1 = get_next_node(n0);
pidtype n2 = get_next_node(n1);
pidtype n3 = get_next_node(n2);
pidtype n4 = get_next_node(n3);

// Print for debugging
printf("Ready list traversal: %u %u %u %u %u\n", n0, n1, n2, n3, n4);
printf("Expected order: p3=%u, p2=%u, p1=%u, p0=%u\n", p3, p2, p1, p0);

assert(n0 == p3);
assert(n1 == p2);
assert(n2 == p1);
assert(n3 == p0);
assert(n4 == p3);

    // The list should be circular and contain exactly 4 nodes (null + 3)
    assert(n4 != 255);
    assert(get_next_node(n4) == n1);

    // Check backward links
    assert(get_previous_node(n1) == n4);
    assert(get_previous_node(n2) == n1);
    assert(get_previous_node(n3) == n2);
    assert(get_previous_node(n4) == n3);

    // Check forward links
    assert(get_next_node(n1) == n2);
    assert(get_next_node(n2) == n3);
    assert(get_next_node(n3) == n4);

    printf("[OK] Process ready list links are correct for 4 nodes (including null process).\n");
    return 0;
}

