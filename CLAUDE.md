# CLAUDE.md

randp is a C library providing an arc4random-compatible API over an AES-CTR PRNG.

Follow COMMENT-STYLE.md when writing a comment, doc block, commit message, or documentation file.

- Overview, design, and dependency version rationale: README.md
- Test targets and their runtimes: tests/README.md
- Benchmark targets and the parameter-tuning workflow: benchmarks/README.md
- Compiler, shared flags, version, and install paths: config.mk
- Targets and flags added per directory: Makefile, tests/Makefile, and benchmarks/Makefile
- Lint checks: .clang-tidy

`make test` only builds the test binaries.  The targets in tests/README.md run them.

benchmarks/randp.hpp mirrors src/randp.c, and benchmarks/aes_ctr_prng.hpp mirrors src/aes_ctr_*_prng.h.  A change to a C source needs the same change in its mirror, or the benchmarks tune parameters for code the library no longer runs.
