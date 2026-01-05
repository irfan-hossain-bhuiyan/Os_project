# 🍚 kacchiOS

A feature-rich, educational baremetal operating system for learning OS fundamentals.

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/irfan-hossain-bhuiyan/Os_project)
[![Platform](https://img.shields.io/badge/platform-x86-lightgrey.svg)](https://github.com/irfan-hossain-bhuiyan/Os_project)

## 📖 Overview

kacchiOS is a comprehensive, bare-metal operating system built from scratch for educational purposes. It provides a complete foundation for learning operating system concepts through hands-on implementation.

### ✨ Current Features

- ✅ **Multiboot-compliant bootloader** - Boots via GRUB/QEMU
- ✅ **Serial I/O driver** (COM1) - Full-featured communication via serial port
- ✅ **Process Management** - Complete process lifecycle with 255 process limit
- ✅ **Preemptive Multitasking** - Round-robin scheduler with configurable time quantum (20ms)
- ✅ **Dynamic Memory Management** - Heap allocation with malloc/free/realloc
- ✅ **Inter-Process Communication** - Message passing and semaphores
- ✅ **Interrupt Handling** - IDT, PIC, and timer interrupts
- ✅ **Context Switching** - Hardware-based context switching
- ✅ **System Calls** - User-level syscall interface
- ✅ **Comprehensive Testing** - Unit tests for all major components
- ✅ **Clean, documented code** - Easy to understand and extend

### 🚧 Potential Extensions

Future enhancements could include:

- 🔄 **Process Aging** - Prevent starvation in scheduling
- 🖥️ **VGA Text Mode** - Visual output beyond serial console
- ⌨️ **Keyboard Driver** - Interactive user input
- 💾 **File System** - Basic storage management
- 🔒 **User/Kernel Mode Separation** - Privilege levels

## 🚀 Quick Start

### Prerequisites

```bash
# On Ubuntu/Debian
sudo apt-get install build-essential qemu-system-x86 gcc-multilib

# On Arch Linux
sudo pacman -S base-devel qemu gcc-multilib

# On macOS
brew install qemu i686-elf-gcc
```

### Build and Run

```bash
# Clone the repository
git clone https://github.com/irfan-hossain-bhuiyan/Os_project.git
cd Os_project

# Build the OS
make clean
make

# Run in QEMU (serial output only)
make run

# Or run with VGA display
make run-vga

# Run all tests
make test
```

You should see:

```text
===========================================
  kacchiOS Feature Showcase
  - Processes & Scheduling
  - Message Passing (send/receive)
  - Semaphores (P/V)
  - Heap (malloc/free)
===========================================

[main] PID=0x01 starting demo...
[main] created consumer PID=0x02
[main] created producer targeting consumer.
[main] created heap_worker.
[producer] PID=0x03 sending to PID=0x02
[consumer] PID=0x02: waiting for messages...
...
```

## 📁 Project Structure

```text
kacchiOS/
├── boot.S              # Bootloader entry point (Assembly)
├── kernel.c            # Kernel initialization
├── main.c              # User-level main process (demo)
├── process.c/h         # Process management
├── process_node.c/h    # Process data structures
├── queue.c/h           # Ready queue implementation
├── stack.c/h           # Stack allocator
├── heap.c/h            # Heap memory manager
├── allocator.c/h       # Memory allocator core
├── bitmap.c/h          # Bitmap for memory tracking
├── sem.c/h             # Semaphore implementation
├── idt.c/h             # Interrupt descriptor table
├── pic.c/h             # Programmable interrupt controller
├── timer.c/h           # Timer interrupt handler
├── system.c/h          # System call interface
├── serial.c/h          # Serial port driver (COM1)
├── vga.c/h             # VGA text mode driver
├── keyboard.c/h        # Keyboard driver (PS/2)
├── monitor.c/h         # System monitoring tools
├── debug.c/h           # Debug utilities
├── string.c/h          # String utility functions
├── types.h             # Basic type definitions
├── io.h                # I/O port operations
├── kacchios.h          # Main OS API header
├── link.ld             # Linker script
├── Makefile            # Build system
├── main.c              # Feature demo
├── main_enhanced.c     # Interactive shell demo
├── test_*.c            # Unit tests
├── docs/               # Documentation
│   ├── Architecture.md # System architecture
│   ├── API.md          # API reference
│   ├── FEATURES.md     # Feature list
│   └── Checklist.md    # Implementation checklist
└── README.md           # This file
```

## 🛠️ Build System

### Makefile Targets

| Command              | Description                                  |
| -------------------- | -------------------------------------------- |
| `make` or `make all` | Build kernel.elf                             |
| `make run`           | Run in QEMU (serial output only)             |
| `make run-vga`       | Run in QEMU (with VGA window)                |
| `make debug`         | Run in debug mode (GDB ready)                |
| `make test`          | Run all unit tests                           |
| `make stack_test`    | Run stack allocator tests                    |
| `make heap_test`     | Run heap allocator tests                     |
| `make process_test`  | Run process manager tests                    |
| `make clean`         | Remove build artifacts                       |

### Debug Mode

To debug the kernel with GDB:

```bash
# Terminal 1: Start QEMU in debug mode
make debug

# Terminal 2: Connect GDB
gdb -ex 'target remote localhost:1234' -ex 'symbol-file target/kernel.elf'
```

## 🎓 Documentation

Comprehensive documentation is available in the `docs/` directory:

- **[Architecture.md](docs/Architecture.md)** - System architecture and design decisions
- **[API.md](docs/API.md)** - Complete API reference with examples
- **[FEATURES.md](docs/FEATURES.md)** - Detailed feature list and capabilities
- **[Checklist.md](docs/Checklist.md)** - Implementation progress tracker

## 📚 Learning Resources

### Recommended Reading

- [XINU OS](https://xinu.cs.purdue.edu/) - Educational OS similar to kacchiOS
- [OSDev Wiki](https://wiki.osdev.org/) - Comprehensive OS development guide
- [The Little OS Book](https://littleosbook.github.io/) - Practical OS development
- [Operating Systems: Three Easy Pieces](https://pages.cs.wisc.edu/~remzi/OSTEP/) - OS concepts textbook

### Related Topics

- x86 Assembly Language
- Memory Management
- Process Scheduling
- System Calls
- Interrupt Handling

## 🎯 What Makes This OS Awesome

### Educational Excellence

- **Clean Code**: Well-structured, commented, easy to understand
- **Comprehensive Docs**: Over 1000 lines of documentation
- **Working Examples**: Multiple demo programs included
- **Test Suite**: Unit tests for critical components

### Feature-Rich

- **Complete Process System**: Creation, scheduling, IPC, termination
- **Advanced Scheduling**: Round-robin with aging (anti-starvation)
- **Flexible IPC**: Both message passing and semaphores
- **Dynamic Memory**: Full malloc/free/realloc support
- **Multiple I/O Drivers**: Serial, VGA, Keyboard

### Developer-Friendly

- **Interactive Shell**: Built-in command interface
- **System Monitor**: Real-time stats and debugging
- **Easy to Extend**: Modular design, clear interfaces
- **Great Build System**: One command to build and run

## 🤝 Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

### Guidelines

1. Keep code simple and educational
2. Add comments explaining complex concepts
3. Follow existing code style
4. Test changes in QEMU before submitting

## 📄 License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

## 👥 Authors

Created by students learning operating system development.

## 🙏 Acknowledgments

- Inspired by XINU OS
- Built with guidance from OSDev community
- Thanks to all contributors

---

**kacchiOS 2.0 - Where Learning Meets Implementation!** 🚀
