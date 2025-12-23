# Makefile for kacchiOS

CC = gcc
LD = ld
AS = as

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -nostdinc \
         -fno-builtin -fno-stack-protector -I.
ASFLAGS = --32
LDFLAGS = -m elf_i386

SRCS_C = kernel.c serial.c string.c
SRCS_ASM = boot.S

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
	@mkdir -p $(TARGET_DIR)
	$(AS) $(ASFLAGS) $< -o $@

run: $(KERNEL_ELF)
	qemu-system-i386 -kernel $(KERNEL_ELF) -m 64M -serial stdio -display none

run-vga: $(KERNEL_ELF)
	qemu-system-i386 -kernel $(KERNEL_ELF) -m 64M -serial mon:stdio

debug: $(KERNEL_ELF)
	qemu-system-i386 -kernel $(KERNEL_ELF) -m 64M -serial stdio -display none -s -S &
	@echo "Waiting for GDB connection on port 1234..."
	@echo "In another terminal run: gdb -ex 'target remote localhost:1234' -ex 'symbol-file $(KERNEL_ELF)'"

clean:
	rm -f $(TARGET_DIR)/*

.PHONY: all run run-vga debug clean

# === Stack and Heap Test Build Rules ===
# Test sources
STACK_TEST_SRC = test_stack.c
HEAP_TEST_SRC = test_heap.c
# Object files for tests (in target directory)
STACK_TEST_OBJ = $(TARGET_DIR)/test_stack.o
HEAP_TEST_OBJ = $(TARGET_DIR)/test_heap.o
STACK_OBJ = $(TARGET_DIR)/stack.o
HEAP_OBJ = $(TARGET_DIR)/heap.o

# Test executables
STACK_TEST = $(TARGET_DIR)/test_stack
HEAP_TEST = $(TARGET_DIR)/test_heap



# Test CFLAGS (simple, allow standard includes)
TEST_CFLAGS = -m32 -O2 -Wall -Wextra -I.

# Build rule for stack.o
$(STACK_OBJ): stack.c
	@echo "[CC][LIB] $< -> $@"
	@mkdir -p $(TARGET_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

# Build rule for heap.o
$(HEAP_OBJ): heap.c
	@echo "[CC][LIB] $< -> $@"
	@mkdir -p $(TARGET_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

# Build rule for stack_test.o
$(STACK_TEST_OBJ): $(STACK_TEST_SRC)
	@echo "[CC][TEST] $< -> $@"
	@mkdir -p $(TARGET_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

# Build rule for heap_test.o
$(HEAP_TEST_OBJ): $(HEAP_TEST_SRC)
	@echo "[CC][TEST] $< -> $@"
	@mkdir -p $(TARGET_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

# Link test_stack executable (test_stack.o + stack.o)
$(STACK_TEST): $(STACK_TEST_OBJ) $(STACK_OBJ)
	@echo "[LD][TEST] $^ -> $@"
	$(CC) $(TEST_CFLAGS) $^ -o $@

# Link test_heap executable (test_heap.o + heap.o)
$(HEAP_TEST): $(HEAP_TEST_OBJ) $(HEAP_OBJ)
	@echo "[LD][TEST] $^ -> $@"
	$(CC) $(TEST_CFLAGS) $^ -o $@

# Run test_stack
stack_test: $(STACK_TEST)
	@echo "[RUN] $<"
	./$(STACK_TEST)

# Run test_heap
heap_test: $(HEAP_TEST)
	@echo "[RUN] $<"
	./$(HEAP_TEST)

# Run all tests
test: stack_test heap_test
	@echo "[RUN] All tests completed."

.PHONY: stack_test heap_test test


