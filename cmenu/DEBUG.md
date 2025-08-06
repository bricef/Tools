# Memory Leak Detection for cmenu

This guide explains how to detect memory leaks and allocation errors in the cmenu project using various tools.

## Prerequisites

### macOS
No additional installation needed - macOS has built-in tools.

### Linux
```bash
# Ubuntu/Debian
sudo apt-get install valgrind

# Arch Linux
sudo pacman -S valgrind
```

## Available Make Targets

### macOS Tools

#### `make asan`
Runs the program with AddressSanitizer:
- Detects memory leaks, buffer overflows, use-after-free
- Very fast and accurate
- Built into Clang/GCC

#### `make asan-test`
Runs AddressSanitizer with sample input for testing.

#### `make leaks`
Runs the program with macOS's built-in `leaks` tool:
- Detects memory leaks at program exit
- No performance impact
- macOS only

### Linux Tools (Valgrind)

#### `make valgrind`
Runs a comprehensive Valgrind analysis with:
- Full leak checking
- Origin tracking
- Verbose output
- Results saved to `valgrind-out.txt`

#### `make valgrind-quick`
Runs a quick Valgrind check with:
- Full leak checking
- Suppressed system library false positives
- Console output only

#### `make valgrind-errors`
Runs Valgrind focusing on memory errors:
- Origin tracking for debugging
- No leak checking (faster)
- Good for finding use-after-free and invalid memory access

## Running Tests

### macOS/Linux - AddressSanitizer
```bash
# Quick test with sample input
make asan-test
```

### macOS - Leaks Tool
```bash
# Test with built-in leaks tool
make leaks-test
```

### Linux - Valgrind
```bash
# Quick test with sample input
make valgrind-test
```

## Tool Comparison

| Tool | Platform | Speed | Accuracy | Setup |
|------|----------|-------|----------|-------|
| AddressSanitizer | All | Fast | High | Easy |
| Leaks | macOS | Very Fast | Medium | None |
| Valgrind | Linux | Slow | High | Easy |
