# Makefile for kacchiOS

CC = gcc
LD = ld
AS = as

# Using -O0 for stable bare-metal development per user request
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -nostdinc \
         -fno-builtin -fno-stack-protector -I. 
#-DKERNEL_DEBUG
ASFLAGS = --32
LDFLAGS = -m elf_i386

SRCS_C = kernel.c serial.c string.c process.c stack.c idt.c pic.c system.c \
         debug.c timer.c heap.c sem.c main.c vga.c keyboard.c monitor.c
SRCS_ASM = boot.S timer_stub.S

TARGET_DIR = target
OBJS = $(patsubst %.c,$(TARGET_DIR)/%.o,$(SRCS_C)) \
       $(patsubst %.S,$(TARGET_DIR)/%.o,$(SRCS_ASM))
KERNEL_ELF = $(TARGET_DIR)/kernel.elf

all: $(KERNEL_ELF)

$(KERNEL_ELF): $(OBJS)
	@mkdir -p $(TARGET_DIR)
	$(LD) $(LDFLAGS) -T link.ld -o $@ $^

$(TARGET_DIR)/%.o: %.c
	@mkdir -p $(TARGET_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET_DIR)/%.o: %.S
	$(AS) $(ASFLAGS) $< -o $@
	@echo "Assembling $<"

run: $(KERNEL_ELF)
	qemu-system-i386 -kernel $(KERNEL_ELF) -m 512M -serial stdio -display none -device isa-debug-exit

run-vga: $(KERNEL_ELF)
	qemu-system-i386 -kernel $(KERNEL_ELF) -m 512M -serial mon:stdio -device isa-debug-exit

debug: $(KERNEL_ELF)
	qemu-system-i386 -kernel $(KERNEL_ELF) -m 512M -serial stdio -display none -device isa-debug-exit -s -S &
	@echo "Waiting for GDB connection on port 1234..."
	@echo "In another terminal run: gdb -ex 'target remote localhost:1234' -ex 'symbol-file $(KERNEL_ELF)'"

clean:
	rm -f $(TARGET_DIR)/*

.PHONY: all run run-vga debug clean

# === Unified Test Build Rules ===

# Test sources
TEST_SRCS = test_stack.c test_heap.c test_process.c
TEST_BINS = $(patsubst %.c,$(TARGET_DIR)/%,$(TEST_SRCS))
TEST_OBJS = $(patsubst %.c,$(TARGET_DIR)/%.o,$(TEST_SRCS))

# All kernel objects except kernel.o and boot.o
KERNEL_OBJS_NO_MAIN = $(filter-out $(TARGET_DIR)/kernel.o $(TARGET_DIR)/boot.o, $(OBJS))

# Test CFLAGS (simple, allow standard includes)
TEST_CFLAGS = -m32 -O0 -Wall -Wextra -I. 
# Pattern rule for test object files
$(TARGET_DIR)/%.o: %.c
	@echo "[CC][TEST] $< -> $@"
	@mkdir -p $(TARGET_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

# Pattern rule for test executables
$(TARGET_DIR)/%: $(TARGET_DIR)/%.o $(KERNEL_OBJS_NO_MAIN)
	@echo "[LD][TEST] $^ -> $@"
	$(CC) $(TEST_CFLAGS) $^ -o $@

# Run individual tests
stack_test: $(TARGET_DIR)/test_stack
	@echo "[RUN] $<"
	./$(TARGET_DIR)/test_stack

heap_test: $(TARGET_DIR)/test_heap
	@echo "[RUN] $<"
	./$(TARGET_DIR)/test_heap

process_test: $(TARGET_DIR)/test_process
	@echo "[RUN] $<"
	./$(TARGET_DIR)/test_process

# Run all tests
test: stack_test heap_test process_test
	@echo "[RUN] All tests completed."

.PHONY: stack_test heap_test process_test test
