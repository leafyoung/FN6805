# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a C++ exercise repository for FN6805: OOP I - C++ Programming. Each numbered directory contains a standalone exercise demonstrating specific C++ concepts.

## Build Commands

```bash
make          # Build all .cpp files into ./main executable
make main-debug  # Build with debug symbols (-O0) for debugging
make clean    # Remove compiled executables
./main        # Run the compiled executable
```

**Compiler:** clang++ with C++17, flags: `-g -Wmost -Werror`

Warnings are treated as errors. The Makefile compiles **all** `.cpp` files found in the repo into a single `main` binary, so only one exercise should have an active `main()` at a time.

## Repository Structure

Each exercise is a self-contained numbered directory:

- `10-hello-world/` — Basic I/O
- `20-simple_calc/` — User input, switch statements
- `30-header-file/` — Header guards, function organization
- `32-measure_time/` — `<chrono>` performance measurement
- `34-return_with_move/` — Move semantics
- `40-fib_recursion/` — Recursion, memoization, lambdas
- `50-binaryinoutfile/` — Binary file I/O, `reinterpret_cast`
- `52-stl/` — STL containers (`vector`, `set`, `map`) and algorithms
- `54-fstream_demo/` — File stream operations
- `60-constexpr/` — Compile-time computation
- `62-rand_dist/` — Random number distributions
- `64-bond_pricer/` — Structured bindings, tuples, financial math
- `70-3n_and_1/` — Mathematical algorithms
- `71-gcd/` — GCD algorithm
- `72-multiple_inclusion/` — Preventing multiple inclusion with `#pragma once`
- `73-cache_locality/` — Memory access patterns and cache efficiency

## Working on an Exercise

Since all `.cpp` files are compiled together, to work on a specific exercise:

1. Ensure only one `main()` function exists across the entire repo (comment out or rename others if needed).
2. Build and run with `make && ./main`.

Header files use `#pragma once` for include guards (see `30-header-file/` and `72-multiple_inclusion/` for examples).

## C++ Standards Used

C++17 features actively used throughout:
- Structured bindings (`auto [a, b] = ...`)
- `constexpr` functions
- Lambda captures and `std::function`
- STL algorithms (`sort`, `find`, `iota`, `binary_search`, `for_each`)
- String literals with `s` suffix (`using namespace std::string_literals`)
- `<chrono>` for timing
