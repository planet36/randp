# Benchmarks

## Dependencies

* An x86-64-v3 processor with AES, VAES, RDRAND, and RDSEED
* [GCC 14](https://gcc.gnu.org/gcc-14/changes.html) or newer
  * The benchmarks use [C++23](https://en.cppreference.com/cpp/23).
  * clang and clang++ are not supported.
* [Google Benchmark](https://github.com/google/benchmark), found through `pkg-config`
* [Glibc 2.36](https://www.phoronix.com/news/GNU-C-Library-Glibc-2.36) or newer
  * [arc4random](https://man7.org/linux/man-pages/man3/arc4random.3.html) functions were added in glibc 2.36, but the interface was <q>added as a basic loop wrapper around `getrandom()`</q>. [^arc4random_1] [^arc4random_2]

[^arc4random_1]: https://lists.gnu.org/archive/html/info-gnu/2022-08/msg00000.html

[^arc4random_2]: https://lore.kernel.org/all/20220726195822.1223048-1-Jason@zx2c4.com/

## Example commands

* `make run-all` (takes about 10.8 minutes)
  * The `mutex` benchmark compares the use of `pthread_mutex_t` and `thread_local`.
  * The `others` benchmark compares randp to these other sources of random numbers:
    * `RDRAND`
    * `RDSEED`
    * `getentropy`
    * `arc4random`
  * The `pool-size`, `prng-params`, and `reseed-countdown` benchmarks compare varying parameters of randp and its internal PRNG, with both the AES (`__m128i`) and VAES (`__m256i`) PRNGs.

## Refine the randp parameters

Run these targets in the following order to refine the parameters of randp (which are set in [randp-defaults.h](../src/randp-defaults.h), [aes_ctr_128_prng-defaults.h](../src/aes_ctr_128_prng-defaults.h), and [aes_ctr_256_prng-defaults.h](../src/aes_ctr_256_prng-defaults.h)).

1. `make pool-size`: find optimal `DEFAULT_RANDP_128_POOL_SIZE_BYTES` and `DEFAULT_RANDP_256_POOL_SIZE_BYTES`
2. `make reseed-countdown`: find optimal `DEFAULT_RANDP_128_RESEED_INTERVAL` (`DEFAULT_RANDP_256_RESEED_INTERVAL` is derived from it)
3. `make prng-params`: find optimal `DEFAULT_AES_CTR_128_PRNG_NUM_KEYS`, `DEFAULT_AES_CTR_256_PRNG_NUM_KEYS`, `DEFAULT_RANDP_PRNG_USE_ENC`, and `DEFAULT_RANDP_PRNG_USE_DAVIES_MEYER`
4. _repeat_

## Benchmark results

The tables below come from `make others`, which runs each benchmark on one thread, and are copied from `results/*/others.filtered.txt`.  The tables for _upper_bound_ use the `rand_lt-many` rows, which cycle _upper_bound_ from 1 to 0x100000, not the `rand_lt-one` rows, which hold it at 0xa5a5.  The benchmarks run in a VirtualBox guest with 8 vCPUs, so the System info lists the host's CPU.

> [!NOTE]
> The [glibc arc4random](https://sourceware.org/git/?p=glibc.git;a=blob;f=stdlib/arc4random.c;h=7818cb9cf66e0f3b428a974c90bee1f120668561;hb=HEAD) is completely different than the [OpenBSD arc4random](https://github.com/openbsd/src/blob/c920a736d2c1ec1bc99322d5576ae084602f0870/lib/libc/crypt/arc4random.c).

> [!NOTE]
> Glibc version 2.41 [added support for getrandom vDSO](https://sourceware.org/bugzilla/show_bug.cgi?id=29437#c17).  This dramatically sped up `arc4random` on Linux. [^getrandom_vdso_1] [^getrandom_vdso_2]

[^getrandom_vdso_1]: https://www.phoronix.com/news/glibc-getrandom-vDSO-Merged

[^getrandom_vdso_2]: https://lwn.net/Articles/983186/

### 2026-11-27

#### System info

- Linux 7.3.6-arch1-1 x86_64
- 13th Gen Intel(R) Core(TM) i9-13980HX
- ldd (GNU libc) 2.44
- gcc (GCC) 16.2.1 20260810
- libbenchmark.so.1.10.0
- randp 12.2

#### Fill a buffer with random bytes

| Function | Median time to generate 4 GiB | |
|---|---:|---|
| `randp_bytes`    |  252  ms | 20.6&times; faster |
| `arc4random_buf` | 5183  ms | |
| `getentropy`     | 7518  ms | |

#### Get a uniform random `uint32_t`

| Function | Median time per call | |
|---|---:|---|
| `randp_u32`  | 7.07  ns | 2.0&times; faster |
| `arc4random` | 14.3  ns | |
| `rdrand32`   |  288  ns | |
| `rdseed32`   | 1837  ns | |

`rdrand32` and `rdseed32` wrap `_rdrand32_step` and `_rdseed32_step`, respectively.

#### Get a uniform random `uint32_t` less than _upper_bound_ = [1 .. 0x100000]

| Function | Median time per call | |
|---|---:|---|
| `randp_lt_u32`       | 7.12  ns | 3.4&times; faster |
| `arc4random_uniform` | 23.9  ns | |
