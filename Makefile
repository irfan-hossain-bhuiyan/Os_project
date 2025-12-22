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
