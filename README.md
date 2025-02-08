# AES-CTR Random Pool (randp)

AES-CTR Random Pool (also known as <q>randp</q>) is a C library, designed for modern Linux systems, with functions that give pseudorandom numbers.  It was inspired by [arc4random](https://en.wikipedia.org/wiki/RC4#RC4-based_random_number_generators), and it mimics its functionality.

## API

The API is designed to be compatible with the arc4random family of functions.

| Description | arc4random family | randp family |
|---|---|---|
| Fill a buffer with _n_ random bytes | `void arc4random_buf(void* buf, size_t n)` | `void randp_bytes(void* buf, size_t n)` |
| Return a uniform random unsigned integer | `uint32_t arc4random()` | `uint8_t randp_u8()`<br />`uint16_t randp_u16()`<br />`uint32_t randp_u32()`<br />`uint64_t randp_u64()` |
| Return a uniform random unsigned integer less than _upper_bound_ | `uint32_t arc4random_uniform(uint32_t upper_bound)` | `uint32_t randp_lt_u32(uint32_t upper_bound)` |

## Design

The randp structure stores random bytes in a pool that can be retrieved later by the functions listed above.

The `static` `thread_local` randp structure is `mmap`d with flags `MAP_PRIVATE | MAP_ANONYMOUS` and then `madvise`d with `MADV_DONTDUMP` and `MADV_WIPEONFORK`.  Its size is at most one page (4096 B), and it's not allocated until immediately before first use.  See [src/allocate.h](src/allocate.h) for details.

<details>

<summary>Description of <code><a href="https://man7.org/linux/man-pages/man2/mmap.2.html">mmap</a></code> flags</summary>

<pre>
MAP_PRIVATE
       Create a private copy-on-write mapping.  Updates to the
       mapping are not visible to other processes mapping the
       same file, and are not carried through to the underlying
       file.  It is unspecified whether changes made to the file
       after the mmap() call are visible in the mapped region.
</pre>

<pre>
MAP_ANONYMOUS
       The mapping is not backed by any file; its contents are
       initialized to zero.  The fd argument is ignored; however,
       some implementations require fd to be -1 if MAP_ANONYMOUS
       (or MAP_ANON) is specified, and portable applications
       should ensure this.  The offset argument should be zero.
       Support for MAP_ANONYMOUS in conjunction with MAP_SHARED
       was added in Linux 2.4.
</pre>

</details>

<details>

<summary>Description of <code><a href="https://man7.org/linux/man-pages/man2/madvise.2.html">madvise</a></code> flags</summary>

<pre>
MADV_DONTDUMP (since Linux 3.4)
       Exclude from a core dump those pages in the range
       specified by addr and length.  This is useful in
       applications that have large areas of memory that are
       known not to be useful in a core dump.  The effect of
       MADV_DONTDUMP takes precedence over the bit mask that is
       set via the /proc/pid/coredump_filter file (see core(5)).
</pre>

<pre>
MADV_WIPEONFORK (since Linux 4.14)
       Present the child process with zero-filled memory in this
       range after a fork(2).  This is useful in forking servers
       in order to ensure that sensitive per-process data (for
       example, PRNG seeds, cryptographic secrets, and so on) is
       not handed to child processes.

       The MADV_WIPEONFORK operation can be applied only to
       private anonymous pages (see mmap(2)).

       Within the child created by fork(2), the MADV_WIPEONFORK
       setting remains in place on the specified address range.
       This setting is cleared during execve(2).
</pre>

</details>

Upon first use, the pseudorandom number generator (PRNG) is initialized/seeded by `getentropy`.  See [src/aes128_prng.h](src/aes128_prng.h) for details.  Then the pool is filled with random bytes generated by the PRNG.

As bytes are retrieved from the pool, they are zeroized.  After a certain number of bytes has been read, the PRNG is reseeded, and the pool is filled again with random bytes.  See [src/randp.c](src/randp.c) for details.

## Benchmark Results

> [!NOTE]
> The [glibc arc4random](https://sourceware.org/git/?p=glibc.git;a=blob;f=stdlib/arc4random.c;h=7818cb9cf66e0f3b428a974c90bee1f120668561;hb=HEAD) is completely different that the [OpenBSD arc4random](https://github.com/openbsd/src/blob/c920a736d2c1ec1bc99322d5576ae084602f0870/lib/libc/crypt/arc4random.c).

> [!NOTE]
> Glibc version 2.41 [added support for getrandom vDSO](https://sourceware.org/bugzilla/show_bug.cgi?id=29437#c17).  This dramatically sped up `getentropy` and `arc4random` on Linux.
See also:
[GNU C Library Merges Support for getrandom vDSO](https://www.phoronix.com/news/glibc-getrandom-vDSO-Merged)
[What became of getrandom() in the vDSO](https://lwn.net/Articles/983186/)


> [!NOTE]
> Glibc version 2.41 [added support for getrandom vDSO](https://sourceware.org/bugzilla/show_bug.cgi?id=29437#c17).  This dramatically sped up `getentropy` and `arc4random` on Linux.
See also:
- [GNU C Library Merges Support for getrandom vDSO](https://www.phoronix.com/news/glibc-getrandom-vDSO-Merged)
- [What became of getrandom() in the vDSO](https://lwn.net/Articles/983186/)


> [!NOTE]
> Glibc version 2.41 [added support for getrandom vDSO](https://sourceware.org/bugzilla/show_bug.cgi?id=29437#c17).  This dramatically sped up `getentropy` and `arc4random` on Linux.
See also:
* [GNU C Library Merges Support for getrandom vDSO](https://www.phoronix.com/news/glibc-getrandom-vDSO-Merged)
* [What became of getrandom() in the vDSO](https://lwn.net/Articles/983186/)


> [!NOTE]
> Glibc version 2.41 [added support for getrandom vDSO](https://sourceware.org/bugzilla/show_bug.cgi?id=29437#c17).  This dramatically sped up `getentropy` and `arc4random` on Linux.
> See also:
> [GNU C Library Merges Support for getrandom vDSO](https://www.phoronix.com/news/glibc-getrandom-vDSO-Merged)
> [What became of getrandom() in the vDSO](https://lwn.net/Articles/983186/)


> [!NOTE]
> Glibc version 2.41 [added support for getrandom vDSO](https://sourceware.org/bugzilla/show_bug.cgi?id=29437#c17).  This dramatically sped up `getentropy` and `arc4random` on Linux.
> See also:
> - [GNU C Library Merges Support for getrandom vDSO](https://www.phoronix.com/news/glibc-getrandom-vDSO-Merged)
> - [What became of getrandom() in the vDSO](https://lwn.net/Articles/983186/)


> [!NOTE]
> Glibc version 2.41 [added support for getrandom vDSO](https://sourceware.org/bugzilla/show_bug.cgi?id=29437#c17).  This dramatically sped up `getentropy` and `arc4random` on Linux.
> See also:
> * [GNU C Library Merges Support for getrandom vDSO](https://www.phoronix.com/news/glibc-getrandom-vDSO-Merged)
> * [What became of getrandom() in the vDSO](https://lwn.net/Articles/983186/)



> [!NOTE]
> Glibc version 2.41 [added support for getrandom vDSO](https://sourceware.org/bugzilla/show_bug.cgi?id=29437#c17).  This dramatically sped up `getentropy` and `arc4random` on Linux.
> <details>
> <summary>See these for more info</summary>
> * [GNU C Library Merges Support for getrandom vDSO](https://www.phoronix.com/news/glibc-getrandom-vDSO-Merged)
> * [What became of getrandom() in the vDSO](https://lwn.net/Articles/983186/)
> </details>


> [!NOTE]
> Glibc version 2.41 [added support for getrandom vDSO](https://sourceware.org/bugzilla/show_bug.cgi?id=29437#c17).  This dramatically sped up `getentropy` and `arc4random` on Linux.
> <details>
> <summary>See these for more info</summary>
> [GNU C Library Merges Support for getrandom vDSO](https://www.phoronix.com/news/glibc-getrandom-vDSO-Merged),
> [What became of getrandom() in the vDSO](https://lwn.net/Articles/983186/)
> </details>



> [!NOTE]
> Glibc version 2.41 [added support for getrandom vDSO](https://sourceware.org/bugzilla/show_bug.cgi?id=29437#c17).  This dramatically sped up `getentropy` and `arc4random` on Linux.
> <details>
> <summary>See these for more info</summary>
> https://www.phoronix.com/news/glibc-getrandom-vDSO-Merged
> https://lwn.net/Articles/983186/
> </details>



### 2025-02-03

#### System Info

- Linux 6.13.1-arch1-1 x86_64
- 13th Gen Intel(R) Core(TM) i9-13980HX
- ldd (GNU libc) 2.41
- gcc (GCC) 14.2.1 20250128
- libbenchmark.so.1.9.1
- librandp.so.4.4

#### Fill a buffer with random bytes

| Function | Median time to generate 4 GiB | |
|---|---:|---|
| `randp_bytes`    |  189  ms | 26.7&times; faster |
| `arc4random_buf` | 5053  ms | |
| `getentropy`     | 7782  ms | |

#### Get a uniform random `uint32_t`

| Function | Median time per call | |
|---|---:|---|
| `randp_u32`  | 6.22  ns | 2.1&times; faster |
| `arc4random` | 13.1  ns | |
| `rdrand32`   |  287  ns | |
| `rdseed32`   | 1809  ns | |

Note: `rdrand32` and `rdseed32` are wrappers for `_rdrand32_step` and `_rdseed32_step`, respectively.

#### Get a uniform random `uint32_t` less than _upper_bound_ = [1 .. 0x100000]

| Function | Median calls per second | |
|---|---:|---|
| `randp_lt_u32`       | 153.104M/s | 3.28&times; more |
| `arc4random_uniform` | 46.6285M/s | |

### 2024-10-04

#### System Info

- Linux 6.11.1-arch1-1 x86_64
- 13th Gen Intel(R) Core(TM) i9-13950HX
- ldd (GNU libc) 2.40
- gcc (GCC) 14.2.1 20240910
- libbenchmark.so.1.9.0
- librandp.so.4.3

#### Fill a buffer with random bytes

| Function | Median time to generate 4 GiB | |
|---|---:|---|
| `randp_bytes`    |  217  ms | 43&times; faster |
| `getentropy`     | 8993  ms | |
| `arc4random_buf` | 9410  ms | |

#### Get a uniform random `uint32_t`

| Function | Median time per call | |
|---|---:|---|
| `randp_u32`  | 6.69  ns | 27&times; faster |
| `arc4random` |  181  ns | |
| `rdrand32`   |  304  ns | |
| `rdseed32`   | 1825  ns | |

Note: `rdrand32` and `rdseed32` are wrappers for `_rdrand32_step` and `_rdseed32_step`, respectively.

#### Get a uniform random `uint32_t` less than _upper_bound_ = [1 .. 0x100000]

| Function | Median calls per second | |
|---|---:|---|
| `randp_lt_u32`       | 149.148M/s | 39&times; more |
| `arc4random_uniform` | 3.7692M/s  | |

## Dependencies

### To build and use randp

* A processor with [AES instructions](https://en.wikipedia.org/wiki/AES_instruction_set)
* [Linux 4.14](https://kernelnewbies.org/Linux_4.14) or newer
  * [`MADV_WIPEONFORK`](https://man7.org/linux/man-pages/man2/madvise.2.html#:~:text=of%20memory%20pressure.-,MADV_WIPEONFORK) was added in Linux 4.14
* [GCC 14](https://gcc.gnu.org/gcc-14/changes.html) or newer
  * [C23](https://en.cppreference.com/w/c/23) support was added in GCC 14.  randp uses the [`thread_local`](https://en.cppreference.com/w/c/keyword/thread_local) keyword.
* [Glibc 2.25](https://www.phoronix.com/news/glibc-2.25-Released) or newer
  * [`getentropy`](https://man7.org/linux/man-pages/man3/getentropy.3.html) was added in glibc 2.25. [^1] [^2]

[^1]: https://sourceware.org/legacy-ml/libc-alpha/2017-02/msg00079.html

[^2]: https://sourceware.org/bugzilla/show_bug.cgi?id=17252#c7

### To run the benchmarks

* [Google Benchmark](https://github.com/google/benchmark)
* [Glibc 2.36](https://www.phoronix.com/news/GNU-C-Library-Glibc-2.36)
  * [arc4random](https://man7.org/linux/man-pages/man3/arc4random.3.html) functions were added in glibc 2.36, but the interface was <q>added as a basic loop wrapper around `getrandom()`</q>. [^3] [^4]

[^3]: https://lists.gnu.org/archive/html/info-gnu/2022-08/msg00000.html

[^4]: https://lore.kernel.org/all/20220726195822.1223048-1-Jason@zx2c4.com/

### To run the tests

* Glibc 2.36
* `RNG_test` from [PractRand](https://github.com/planet36/PractRand)
  * This is a _very_ lengthy test, taking about 1 hour for each test run!  To make it quicker (yet less thorough), reduce the values of the `-tf`, `-te`, and `-tlmax` options to `RNG_test`.
* [Valgrind](https://valgrind.org/)
  * [KCachegrind](https://apps.kde.org/kcachegrind/) or QCachegrind to view the output
* `rngtest` from [rng-tools](https://github.com/nhorman/rng-tools)
* [openssl-rand](https://docs.openssl.org/master/man1/openssl-rand/)

## Tests

randp is fork-safe, just like arc4random.

randp passes PractRand tests through 512 GiB with these exhaustive options: <q>-tf 2</q>, <q>-te 1</q>.  And it scores a similar number of FIPS 140-2 successes and failures as arc4random and openssl-rand.

It has not been tested with [TestU01](https://en.wikipedia.org/wiki/TestU01) or [diehard](https://en.wikipedia.org/wiki/Diehard_tests).

## Is randp a cryptographically secure pseudorandom number generator ([CSPRNG](https://en.wikipedia.org/wiki/Cryptographically_secure_pseudorandom_number_generator))?

No claims are made that randp is a CSPRNG, but the random numbers it produces are practically unpredictable (i.e. computationally infeasible to predict).  Of the following critera of a CSPRNG, it hasn't satisfied numbers 6, 7, or 8.

<blockquote>
To prove that a random number generator (RNG) is cryptographically secure, it must meet several stringent criteria and undergo rigorous testing. Here's an overview of the key aspects involved in proving cryptographic security for an RNG:

1. **Unpredictability**
   - **Next-bit unpredictability:** Given the first _k_ bits of a random sequence, it should be computationally infeasible to predict the _k_+1-th bit with any significant advantage over random guessing (which would be 50% for a single bit).
   - **Forward and backward security:** Even if part of the state or some outputs are compromised, the RNG should still be secure. This means that past outputs can't be reconstructed (backward security) and future outputs can't be predicted (forward security).

2. **Statistical Randomness Tests**
   - The output of the RNG must pass a battery of statistical tests that measure the randomness of the sequence. Common tests include the NIST Statistical Test Suite, Diehard tests, and TestU01.
   - While these tests don't prove cryptographic security, they help ensure the output appears random, which is a necessary condition.

3. **Entropy Source**
   - A cryptographically secure RNG must be based on a source of true entropy. This could be physical processes (like thermal noise or radioactive decay) or carefully designed algorithms that maintain high entropy.
   - The entropy must be unpredictable and cannot be influenced or predicted by an attacker.

4. **Resilience to Attacks**
   - **Mathematical Proofs:** The design of the RNG should ideally be accompanied by mathematical proofs demonstrating that it is resistant to known cryptographic attacks, such as linear or differential cryptanalysis.
   - **Seed Security:** The seed used to initialize the RNG must be secure. If the seed is predictable, the entire RNG is compromised.

5. **Cryptographic Algorithms**
   - The RNG should utilize cryptographic primitives like block ciphers (e.g., AES), cryptographic hash functions (e.g., SHA-2), or stream ciphers that are proven secure under standard assumptions (e.g., security of AES against known attacks).
   - The design should avoid known weaknesses such as bias in output or correlations between bits.

6. **Security Proofs**
   - A formal security proof can provide strong evidence of cryptographic security. This proof would typically show that breaking the RNG's security is as hard as solving a known difficult problem (e.g., factoring large integers or solving discrete logarithms), which is assumed to be infeasible within polynomial time for large instances.

7. **Independent Verification**
   - The RNG should undergo independent security analysis and audits by third parties. This is to ensure that there are no hidden flaws or vulnerabilities that were overlooked by the original designers.

8. **Compliance with Standards**
   - The RNG should comply with established cryptographic standards like those provided by NIST (e.g., NIST SP 800-90A/B/C) or other recognized bodies. These standards provide guidelines for the design, testing, and validation of cryptographically secure RNGs.

**Summary**

Proving that an RNG is cryptographically secure involves demonstrating that it is unpredictable, passes statistical tests, has a high-entropy source, is resilient to attacks, and follows cryptographic principles backed by security proofs. It should also undergo independent verification and comply with relevant standards.
</blockquote>

&mdash; <cite>ChatGPT (GPT-4o)</cite>. <q>How Can a Random Number Generator Be Proven to Be Cryptographically Secure?</q> *OpenAI*, 2024-08-28.

## License

[The Open Software License 3.0](https://opensource.org/license/osl-3-0-php) (OSL-3.0)
