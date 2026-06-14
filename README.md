# TicketMaster

A simple C++17 ticket reservation system that demonstrates concurrent seat holds, confirmation, and expiration handling.

## Features

- Reserve seats with a configurable hold timeout
- Confirm reserved seats for the correct user
- Automatically expire unconfirmed reservations in a background worker
- Thread-safe seat access and expiry scheduling
- Basic demo application and unit-style test executable

## Project Structure

- `/include/TicketSystem.h` — public `TicketSystem` interface
- `/src/TicketSystem.cpp` — core implementation
- `/src/main.cpp` — demo executable
- `/tests/test_ticket_system.cpp` — test executable

## Requirements

- CMake 3.16+
- C++17 compatible compiler (e.g., GCC/Clang)

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run Demo

```bash
./build/demo
```

## Run Tests

```bash
ctest --test-dir build
```

Or run the test binary directly:

```bash
./build/test_ticket_system
```
