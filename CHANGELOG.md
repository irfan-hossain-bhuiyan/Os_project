# Changelog

All notable changes to kacchiOS are documented in this file.

## [2.0.0] - Enhanced Edition - 2026-01-05

### 🎉 Major New Features

#### Process Management Enhancements

- **Process Aging**: Implemented aging mechanism to prevent process starvation
  - Tracks age (waiting time) for each process
  - Prioritizes oldest waiting processes
  - Ensures fair CPU time distribution
- **CPU Time Tracking**: Added `total_ticks` counter per process
  - Enables CPU usage statistics
  - Helps identify CPU-intensive processes

#### New I/O Drivers

- **VGA Text Mode Driver** (`vga.c/h`)
  - 80x25 character display
  - 16 foreground and 8 background colors
  - Cursor control and positioning
  - Screen scrolling
  - Hexadecimal and decimal output
  - Full character set support

- **Keyboard Driver** (`keyboard.c/h`)
  - PS/2 keyboard support
  - Interrupt-driven input
  - 128-byte circular buffer
  - Modifier key support (Shift, Ctrl, Alt, Caps Lock)
  - Scancode to ASCII conversion
  - US QWERTY layout
  - Blocking and non-blocking read modes

#### System Monitoring Tools

- **System Monitor** (`monitor.c/h`)
  - Process table viewer with state and statistics
  - Ready queue visualization
  - CPU usage per process
  - Memory usage statistics (heap and stack)
  - System information display
  - Process information dump
  - Memory hex dump utility

#### Memory Management Improvements

- **Heap Statistics**: Added `heap_stats()` function
  - Reports used, free, and total heap memory
  - Calculates utilization percentage
- **Stack Usage Tracking**: Added `get_stack_usage()` function
  - Counts allocated stacks
  - Reports total stack memory used

### 📚 Documentation

- **Architecture Documentation** (`docs/Architecture.md`)
  - Complete system architecture overview
  - Design decisions explained
  - Memory layout diagrams
  - Component interactions

- **API Reference** (`docs/API.md`)
  - Comprehensive API documentation
  - Function signatures and parameters
  - Return values and error codes
  - Usage examples and patterns
  - Code snippets

- **Features Document** (`docs/FEATURES.md`)
  - Detailed feature descriptions
  - Performance characteristics
  - Educational value
  - Future enhancement ideas

- **Enhanced README**
  - Updated with all new features
  - Better project structure documentation
  - Comprehensive build instructions
  - Learning resources

- **Updated Checklist** (`docs/Checklist.md`)
  - Marked all completed features
  - Added new feature categories
  - Organized by component

### 🎨 Demo Programs

- **Enhanced Interactive Demo** (`main_enhanced.c`)
  - Interactive shell with commands
  - Process management commands (ps, kill)
  - Memory monitoring (mem)
  - CPU usage display (cpu)
  - System info (info, stats)
  - Process dump (dump &lt;pid&gt;)
  - Queue visualization (queue)

### 🔧 Build System

- **Updated Makefile**
  - Added VGA driver compilation
  - Added keyboard driver compilation
  - Added monitor utilities compilation
  - All new modules integrated

### 🐛 Bug Fixes

- Fixed potential issues in process cleanup
- Improved error handling in memory allocation
- Better interrupt handling for new drivers

### ⚡ Performance Improvements

- Optimized scheduler with aging for better fairness
- Improved context switch efficiency
- Better memory allocation with statistics tracking

## [1.0.0] - Initial Release

### Core Features

- Basic multiboot-compliant bootloader
- Serial I/O driver (COM1)
- Process management (up to 254 processes)
- Round-robin scheduler
- Basic context switching
- Stack allocation (4KB per process)
- Heap allocation (bump allocator)
- Inter-process communication:
  - Message passing
  - Semaphores
- Interrupt handling (IDT, PIC, Timer)
- System call interface
- String utilities
- Debug utilities
- Basic test suite

## Version Comparison

| Feature                    | v1.0 | v2.0 |
| -------------------------- | ---- | ---- |
| Process Management         | ✅   | ✅   |
| Process Aging              | ❌   | ✅   |
| CPU Tracking               | ❌   | ✅   |
| Serial Driver              | ✅   | ✅   |
| VGA Driver                 | ❌   | ✅   |
| Keyboard Driver            | ❌   | ✅   |
| System Monitor             | ❌   | ✅   |
| Heap Statistics            | ❌   | ✅   |
| Stack Statistics           | ❌   | ✅   |
| Interactive Shell          | ❌   | ✅   |
| Architecture Docs          | ❌   | ✅   |
| API Reference              | ❌   | ✅   |
| Features Documentation     | ❌   | ✅   |

## Future Roadmap

### Version 2.1 (Planned)

- User/kernel mode separation
- Priority-based scheduling
- Enhanced shell with more commands
- File system support

### Version 3.0 (Future)

- Virtual memory with paging
- Multi-core support (SMP)
- Network stack
- Advanced graphics

## Statistics

### Version 2.0

- **Total Source Files**: 30+ files
- **Lines of Code**: ~3500+
- **Documentation**: 1500+ lines
- **Features**: 40+ features implemented
- **Test Coverage**: Core components

### Version 1.0

- **Total Source Files**: 20 files
- **Lines of Code**: ~2000
- **Documentation**: 300 lines
- **Features**: 20 features

## Contributors

Thanks to all contributors who have helped make kacchiOS better!

## Links

- Repository: <https://github.com/irfan-hossain-bhuiyan/Os_project>
- Documentation: See `docs/` directory
- Issues: Report bugs and suggest features on GitHub

---

**kacchiOS** - *Making OS Development Accessible and Fun!*
