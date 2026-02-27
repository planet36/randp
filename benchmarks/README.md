# Benchmarks

## Dependencies

* [GCC 14](https://gcc.gnu.org/gcc-14/changes.html) or newer
  * [C++20](https://en.cppreference.com/w/cpp/20.html)
  * clang and clang++ are not supported
* [Google Benchmark](https://github.com/google/benchmark)
* [Glibc 2.36](https://www.phoronix.com/news/GNU-C-Library-Glibc-2.36)
  * [arc4random](https://man7.org/linux/man-pages/man3/arc4random.3.html) functions were added in glibc 2.36, but the interface was <q>added as a basic loop wrapper around `getrandom()`</q>. [^arc4random_1] [^arc4random_2]

[^arc4random_1]: https://lists.gnu.org/archive/html/info-gnu/2022-08/msg00000.html

[^arc4random_2]: https://lore.kernel.org/all/20220726195822.1223048-1-Jason@zx2c4.com/

## Example commands

`make mutex num-blocks others prng-params reseed-countdown`
  - The `mutex` benchmark compares the use of `pthread_mutex_t` and `thread_local`.
  - The `others` benchmark compares randp to these PRNGs:
    - `RDRAND`
    - `RDSEED`
    - `getentropy`
    - `arc4random`
  - The `num-blocks`, `prng-params`, and `reseed-countdown` benchmarks compare varying parameters of randp and its internal PRNG.
  - All the benchmarks take about 10 minutes.

## Refine the randp parameters

Run these targets in the following order to refine the parameters of randp.

1. `make num-blocks`: find optimal `DEFAULT_RANDP_NUM_BLOCKS`
2. `make reseed-countdown`: find optimal `DEFAULT_RANDP_RESEED_COUNTDOWN_MIN`
3. `make prng-params`: find optimal `DEFAULT_AESCTR128_PRNG_NUM_KEYS` and `DEFAULT_AESCTR128_PRNG_NUM_ROUNDS_PER_KEY`
4. _repeat_

## Benchmark Results

> [!NOTE]
> The [glibc arc4random](https://sourceware.org/git/?p=glibc.git;a=blob;f=stdlib/arc4random.c;h=7818cb9cf66e0f3b428a974c90bee1f120668561;hb=HEAD) is completely different that the [OpenBSD arc4random](https://github.com/openbsd/src/blob/c920a736d2c1ec1bc99322d5576ae084602f0870/lib/libc/crypt/arc4random.c).

> [!NOTE]
> Glibc version 2.41 [added support for getrandom vDSO](https://sourceware.org/bugzilla/show_bug.cgi?id=29437#c17).  This dramatically sped up `getentropy` and `arc4random` on Linux. [^getrandom_vdso_1] [^getrandom_vdso_2]

[^getrandom_vdso_1]: https://www.phoronix.com/news/glibc-getrandom-vDSO-Merged

[^getrandom_vdso_2]: https://lwn.net/Articles/983186/

### 2026-02-27

#### System Info

- Linux 6.18.13-arch1-1 x86_64
- 13th Gen Intel(R) Core(TM) i9-13980HX
- ldd (GNU libc) 2.43
- gcc (GCC) 15.2.1 20260209
- libbenchmark.so.1.9.5
- librandp.so.4.5

#### Fill a buffer with random bytes

| Function | Median time to generate 4 GiB | |
|---|---:|---|
| `randp_bytes`    |  199  ms | 25.9&times; faster |
| `arc4random_buf` | 5154  ms | |
| `getentropy`     | 7905  ms | |

#### Get a uniform random `uint32_t`

| Function | Median time per call | |
|---|---:|---|
| `randp_u32`  | 6.33  ns | 2.3&times; faster |
| `arc4random` | 14.5  ns | |
| `rdrand32`   |  288  ns | |
| `rdseed32`   | 1816  ns | |

Note: `rdrand32` and `rdseed32` are wrappers for `_rdrand32_step` and `_rdseed32_step`, respectively.

#### Get a uniform random `uint32_t` less than _upper_bound_ = [1 .. 0x100000]

| Function | Median time per call | |
|---|---:|---|
| `randp_lt_u32`       | 6.32  ns | 3.2&times; more |
| `arc4random_uniform` | 20.3  ns | |

### 2025-10-09

#### System Info

- Linux 6.17.1-arch1-1 x86_64
- 13th Gen Intel(R) Core(TM) i9-13980HX
- ldd (GNU libc) 2.42
- gcc (GCC) 15.2.1 20250813
- libbenchmark.so.1.9.4
- librandp.so.4.4

#### Fill a buffer with random bytes

| Function | Median time to generate 4 GiB | |
|---|---:|---|
| `randp_bytes`    |  202  ms | 25.4&times; faster |
| `arc4random_buf` | 5139  ms | |
| `getentropy`     | 7974  ms | |

#### Get a uniform random `uint32_t`

| Function | Median time per call | |
|---|---:|---|
| `randp_u32`  | 6.65  ns | 2.3&times; faster |
| `arc4random` | 15.3  ns | |
| `rdrand32`   |  286  ns | |
| `rdseed32`   | 1813  ns | |

Note: `rdrand32` and `rdseed32` are wrappers for `_rdrand32_step` and `_rdseed32_step`, respectively.

#### Get a uniform random `uint32_t` less than _upper_bound_ = [1 .. 0x100000]

| Function | Median calls per second | |
|---|---:|---|
| `randp_lt_u32`       | 158.683M/s | 3.6&times; more |
| `arc4random_uniform` | 43.9428M/s | |

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
