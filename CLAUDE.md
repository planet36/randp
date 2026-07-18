# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**randp** is a high-performance C library for generating pseudorandom numbers using AES-CTR (Counter Mode) as the underlying PRNG. It provides an `arc4random`-compatible API and is designed to be fast, fork-safe, and cryptographically secure on modern Linux x86-64 systems.

## Build Commands

```bash
# Build static and shared libraries (default target)
make

# Generate single-header version (randp-single.h)
make randp-single.h

# Clean build artifacts
make clean

# Run clang-tidy linter
make lint

# Install to system (default: /usr/local)
make install

# Uninstall from system
make uninstall
```

**Requirements:** GCC 14+ (clang not supported), C23 (`-std=c23`), `-maes` flag (AES-NI instructions required), Linux 4.14+, Glibc 2.25+ (`explicit_bzero` required).

## Tests

Tests live in `tests/` and dynamically link against the built shared library. Build the main library first.

```bash
# Build all test executables
cd tests && make

# Verify fork-safety
cd tests && make fork

# Generate random bytes (compare randp vs arc4random vs mrand48)
cd tests && make generate

# Test header-only compilation
cd tests && make single

# Exhaustive randomness tests (~70 minutes)
cd tests && make run-PractRand

# FIPS 140-2 testing (~5 minutes)
cd tests && make run-rngtest
```

Tests set `LD_LIBRARY_PATH=..` automatically to find the built `librandp.so`.

## Benchmarks

Benchmarks live in `benchmarks/` and statically link against `librandp.a`. Uses Google Benchmark framework.

```bash
cd benchmarks && make mutex           # thread_local vs pthread_mutex_t
cd benchmarks && make num-blocks      # Optimize DEFAULT_RANDP_NUM_BLOCKS
cd benchmarks && make others          # Compare vs RDRAND, RDSEED, getentropy, arc4random
cd benchmarks && make prng-params     # Optimize AES PRNG parameters
cd benchmarks && make reseed-countdown # Optimize reseed interval
```

## Architecture

### Core Design Pattern

Each thread gets its own `randp` structure via `thread_local` storage. The structure holds a **pool of random bytes** backed by `mmap`-allocated memory:
- Memory is advised `MADV_DONTDUMP` (excluded from core dumps) and `MADV_WIPEONFORK` (zeroed in forked children for fork-safety)
- The pool fits within a single 4096-byte page
- Bytes are zeroized as consumed

### Key Source Files

- `src/randp.c` — Public API implementation: `randp_bytes()`, `randp_u8/16/32/64()`, `randp_lt_u32()`
- `src/aes_ctr_128_prng.h` — AES-CTR-128 PRNG engine (multiple keys/rounds, optional Davies-Meyer compression)
- `src/aes128-utils.h` — Intel SIMD AES instruction wrappers (`__m128i`, `_mm_aesenc_si128`, etc.)
- `src/allocate.h` — Secure `mmap`/`madvise` memory allocation
- `src/nearlydivisionless.h` — Daniel Lemire's nearly-divisionless algorithm for unbiased bounded integers
- `src/randp-defaults.h` — Tunable defaults: pool size (`NUM_BLOCKS=16` → 256 bytes), reseed interval (`1024` pool regenerations)
- `src/aes_ctr_128_prng-defaults.h` — AES PRNG tunable defaults: `NUM_KEYS=3`, `NUM_ROUNDS_PER_KEY=1`
- `randp.h` — Public header (C/C++ compatible, placed in root)

### PRNG Lifecycle

1. First use triggers lazy init: pool allocated via `mmap`, seeded via `getentropy`
2. Pool is filled by AES-CTR encryption in 16-byte (`__m128i`) blocks
3. After `reseed_countdown` pool regenerations (with `rdtsc()` jitter), `getentropy` is called again
4. Fork detected implicitly via `MADV_WIPEONFORK`: child's zeroed memory triggers re-initialization

### Single-Header Version

`amalgamate.py` combines `src/randp.c` and all headers into `randp-single.h`. Guard macro is `RANDP_SINGLE_HEADER`.
