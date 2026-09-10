# AGENTS.md

This file provides guidance to harness when working with code in this repository.

## Project Overview

This is a C++ example repository for FN6805: OOP I - C++ Programming. Each numbered directory contains a standalone example demonstrating specific C++ concepts.

## Build Commands

```bash
make          # Build all .cpp files into ./main executable
make main-debug  # Build with debug symbols (-O0) for debugging
make clean    # Remove compiled executables
./main        # Run the compiled executable
```

**Compiler:** clang++ with C++17, flags: `-g -Wmost -Werror`

Warnings are treated as errors. The Makefile compiles **all** `.cpp` files found in the repo into a single `main` binary, so only one exercise should have an active `main()` at a time.

## Smoke Testing All Projects

```bash
./smoke_test.sh                 # build and run every project
./smoke_test.sh 52 64           # only projects matching 52 or 64
./smoke_test.sh --build-only    # compile only, skip the run phase
./smoke_test.sh --verbose       # echo each program's stdout/stderr
./smoke_test.sh --timeout 120   # per-program run timeout, seconds
./smoke_test.sh --help          # all options
```

`smoke_test.sh` compiles **each directory in isolation** with the same flags as
the Makefile, so every project can be tested in one pass despite each having its
own `main()`. A project passes if it compiles without warnings and then runs to
completion without crashing — output is never checked for correctness.

Details worth knowing before changing the script:

- Binaries, build logs, and run sandboxes go to `$TMPDIR/fn6805-smoke`
  (override with `SMOKE_DIR`), so a run never dirties `git status`.
- Every project also produces a machine-readable record in
  `$SMOKE_DIR/results.jsonl` — one JSON object per project with `status`,
  `detail`, `duration_ms`, and log paths, plus a final `run_summary` line. Parse
  that rather than scraping the human-readable table.
- `status` is one of `pass`, `xfail`, `xpass`, `build_fail`, `crash`, `timeout`,
  `exit`, or `harness` (the last meaning the runner itself failed, not the
  program).
- The script degrades instead of breaking on non-GNU systems: it accepts
  `timeout` or `gtimeout` and warns if neither exists, and times runs with bash's
  `EPOCHREALTIME` rather than `date +%N`.
- Each program runs inside a disposable **copy** of its directory. Several
  examples write data files (`54-fstream_demo` writes `test.txt`/`test2.txt`,
  `50-binaryinoutfile` writes `out_test.dat`); the sandbox keeps those out of the
  working tree.
- Examples disagree about their working directory: most open `"./data.txt"`
  relative to their own directory, while `50-binaryinoutfile` opens
  `"50-binaryinoutfile/out_test.dat"` relative to the repo root (matching the
  Makefile's single-binary layout). The sandbox contains a self-referential
  symlink so both spellings resolve.
- `stdin` is `/dev/null`, so interactive examples such as `20-simple_calc` hit
  EOF and exit instead of blocking.
- The intentionally-broken projects are listed in `EXPECTED_BUILD_FAILURES` and
  reported as `XFAIL`. If one of them ever compiles, the script fails with
  `XPASS` — a signal that the lesson has been accidentally fixed away.
- Exit status is 0 only when every project passes and both expected failures
  still fail.

## Repository Structure

Each example is a self-contained numbered directory. Every directory holds
exactly one `main()`, and its `.cpp` files are never nested in subdirectories.

- `08-enum-class-iterator/` — `enum class`, iterating over enumerators
- `09-memory_lego/` — `sizeof`, object addresses, memory layout
- `10-hello-world/` — Basic I/O
- `11-voucher/` — Classes, constructors, copy semantics
- `20-simple_calc/` — User input, switch statements
- `30-header-file/` — Header guards, function organization (intentionally broken)
- `32-measure_time/` — `<chrono>` performance measurement
- `34-return_with_move/` — Move semantics
- `35-return_tuple/` — Returning multiple values with `std::tuple`
- `36-line_number/` — `__LINE__`/`__FILE__` macros, STL exploration
- `37-function-object/` — Functors and `operator()`
- `40-fib_recursion/` — Recursion, memoization, lambdas
- `50-binaryinoutfile/` — Binary file I/O, `reinterpret_cast`
- `51-iterator-demo/` — Iterator categories and traversal
- `52-stl/` — STL containers (`vector`, `set`, `map`) and algorithms
- `54-fstream_demo/` — File stream operations
- `60-constexpr/` — Compile-time computation
- `62-rand_dist/` — Random number distributions
- `64-bond_pricer/` — Structured bindings, tuples, financial math
- `65-bond_pricer_full/` — Multi-file bond pricer (YTM, zero curve, rate curve)
- `70-3n_and_1/` — Mathematical algorithms
- `71-gcd/` — GCD algorithm
- `72-multiple_inclusion/` — Preventing multiple inclusion with `#pragma once` (intentionally broken)
- `73-cache_locality/` — Memory access patterns and cache efficiency

## Working on an Example

Since all `.cpp` files are compiled together, to work on a specific example:

1. Ensure only one `main()` function exists across the entire repo (comment out or rename others if needed).
2. Build and run with `make && ./main`.

To build and run a single example without touching the rest of the repo, use the
smoke test instead — it compiles one directory at a time and needs no
commenting-out: `./smoke_test.sh 52 --verbose`.

Header files use `#pragma once` for include guards (see `30-header-file/` and `72-multiple_inclusion/` for examples).

## Intentionally Uncompilable Examples

The following projects are **intentionally broken** as teaching examples. They demonstrate what goes wrong when you put function definitions in header files. **Do not fix them** - the link errors are the lesson. They are listed in
`EXPECTED_BUILD_FAILURES` in `smoke_test.sh`, which reports them as `XFAIL`.

- `30-header-file/` - Function definitions in `a_func.h` and `b_func.h` cause "multiple definition" linker errors when the headers are included from multiple translation units (`main.cpp` and `c_func.cpp`). The comment in `main.cpp` explains the issue.
- `72-multiple_inclusion/` - Function definition in `add.h` causes "multiple definition" linker error when included from both `main.cpp` and `use_add.cpp`. The `add.cpp.txt` file is provided as the fix students can apply (rename to `.cpp` and remove the definition from `add.h`). The comment in `main.cpp` explains the issue and the fix.

## Source Header Links

Each example's `main.cpp` opens with a link to that directory on GitHub:

```cpp
// https://github.com/leafyoung/FN6805/tree/main/62-rand_dist
```

These replaced the original `replit.com/@YeKunlun/...` links. The Replit slugs
squashed the directory name (`62-randdist` for `62-rand_dist`), so when adding a
link, derive it from the **directory name**, not from any old slug.

## C++ Standards Used

C++17 features actively used throughout:

- Structured bindings (`auto [a, b] = ...`)
- `constexpr` functions
- Lambda captures and `std::function`
- STL algorithms (`sort`, `find`, `iota`, `binary_search`, `for_each`)
- String literals with `s` suffix (`using namespace std::string_literals`)
- `<chrono>` for timing
