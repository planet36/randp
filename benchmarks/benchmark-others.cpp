// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

// {{{ Copied from rdrand.h
// https://github.com/planet36/dotfiles/blob/main/link/.local/include/rdrand.h

#include <immintrin.h>
#include <stdint.h>

// _rd{rand,seed}{16,32,64}_step returns 0 if a random value was NOT generated

#if defined(__RDRND__)
static uint16_t
rdrand16()
{
    unsigned short ret = 0;
    static_assert(sizeof(ret) * 8 == 16);
    while (_rdrand16_step(&ret) == 0)
    {}
    return ret;
}

static uint32_t
rdrand32()
{
    unsigned int ret = 0;
    static_assert(sizeof(ret) * 8 == 32);
    while (_rdrand32_step(&ret) == 0)
    {}
    return ret;
}

static uint64_t
rdrand64()
{
    unsigned long long ret = 0;
    static_assert(sizeof(ret) * 8 == 64);
    while (_rdrand64_step(&ret) == 0)
    {}
    return ret;
}
#else
#warning "__RDRND__ not defined"
#endif

#if defined(__RDSEED__)
static uint16_t
rdseed16()
{
    unsigned short ret = 0;
    static_assert(sizeof(ret) * 8 == 16);
    while (_rdseed16_step(&ret) == 0)
    {}
    return ret;
}

static uint32_t
rdseed32()
{
    unsigned int ret = 0;
    static_assert(sizeof(ret) * 8 == 32);
    while (_rdseed32_step(&ret) == 0)
    {}
    return ret;
}

static uint64_t
rdseed64()
{
    unsigned long long ret = 0;
    static_assert(sizeof(ret) * 8 == 64);
    while (_rdseed64_step(&ret) == 0)
    {}
    return ret;
}
#else
#warning "__RDSEED__ not defined"
#endif

// }}}

#include "../randp.h"

// https://github.com/google/benchmark

#include <benchmark/benchmark.h>
#include <bit>
#include <concepts>

template <std::unsigned_integral T>
using func_T_void_t = T (&)();

template <std::unsigned_integral T>
void
BM_rand_uint(benchmark::State& BM_state, const func_T_void_t<T>& fn)
{
    // Perform setup here

    for (auto _ : BM_state)
    {
        // This code gets timed

        benchmark::DoNotOptimize(fn());
    }
}

template <std::unsigned_integral T>
using func_T_T_t = T (&)(const T);

template <std::unsigned_integral T>
void
BM_rand_lt_one(benchmark::State& BM_state, const func_T_T_t<T>& fn)
{
    // Perform setup here

    // a perfectly cromulent upper bound
    constexpr unsigned int upper_bound = 0xa5a5; // should not be power of 2

    for (auto _ : BM_state)
    {
        // This code gets timed

        benchmark::DoNotOptimize(fn(upper_bound));
    }
}

template <std::unsigned_integral T>
void
BM_rand_lt_many(benchmark::State& BM_state, const func_T_T_t<T>& fn)
{
    // Perform setup here

    // Do not test upper_bound = 0
    constexpr T min_upper_bound = 1;

    // a perfectly cromulent maximum upper bound
    constexpr T max_upper_bound = 0x100000;

    T upper_bound = min_upper_bound;

    for (auto _ : BM_state)
    {
        // This code gets timed

        benchmark::DoNotOptimize(fn(upper_bound));

        ++upper_bound;
        if (upper_bound > max_upper_bound)
            upper_bound = min_upper_bound;
    }
}

using func_t = void (&)(void*, size_t);

void
BM_rand_bytes(benchmark::State& BM_state,
              const func_t& fn,
              const size_t buf_size)
{
    // Perform setup here

    uint8_t* buf = new uint8_t[buf_size];

    for (auto _ : BM_state)
    {
        // This code gets timed

        fn(buf, buf_size);
    }

    delete[] buf;

    BM_state.SetBytesProcessed(BM_state.iterations() * buf_size / static_cast<double>(BM_state.threads()));
}

// This is needed because getentropy returns int, but our interface returns nothing.
inline void
getentropy_wrapper(void* buf, size_t len)
{
    (void)::getentropy(buf, len);
}

void
BM_rand_bytes_4GiB(benchmark::State& BM_state,
                   const func_t& fn)
{
    // Perform setup here

    uint8_t buf[1U << 8];
    static_assert(sizeof(buf) <= 256,
                  "getentropy will fail if more than 256 bytes are requested");
    static_assert(std::has_single_bit(sizeof(buf)), "buffer size must be a power of 2");

    for (auto _ : BM_state)
    {
        // This code gets timed

        for (size_t i = 0; i < (1UL << 32) / sizeof(buf); ++i)
        {
            fn(buf, sizeof(buf));
        }
    }
}

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <string>
#include <thread>

int
main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    // Copied from benchmark.h
    benchmark::MaybeReenterWithoutASLR(argc, argv);
    benchmark::Initialize(&argc, argv);

    if (benchmark::ReportUnrecognizedArguments(argc, argv))
        return 1;

    // {{{ determine num_threads

    constexpr unsigned int min_threads = 1;
    const unsigned int max_threads = std::max(min_threads, std::thread::hardware_concurrency());
    // https://en.wikipedia.org/wiki/Elvis_operator
    //const unsigned int max_threads = std::thread::hardware_concurrency() ?: min_threads;

    unsigned int num_threads;

    try
    {
        num_threads = static_cast<unsigned int>(std::stoi(std::getenv("NUM_THREADS")));
    }
    catch (...)
    {
        num_threads = min_threads;
    }

    num_threads = std::clamp(num_threads, min_threads, max_threads);

    /*
    if (num_threads > min_threads)
        // Don't use all the cores
        --num_threads;
    */

    // }}}

    // {{{ accuracy testing

    // }}}

    // {{{ speed

    std::string prefix;
    size_t buf_size;

    if (num_threads == 1)
    {
        prefix = "rand_uint:8b:";
        benchmark::RegisterBenchmark(prefix + "randp_u8", BM_rand_uint<uint8_t>, randp_u8);

        prefix = "rand_uint:16b:";
#if defined(__RDRND__)
        benchmark::RegisterBenchmark(prefix + "rdrand16", BM_rand_uint<uint16_t>, rdrand16);
#endif
#if defined(__RDSEED__)
        benchmark::RegisterBenchmark(prefix + "rdseed16", BM_rand_uint<uint16_t>, rdseed16);
#endif
        benchmark::RegisterBenchmark(prefix + "randp_u16", BM_rand_uint<uint16_t>, randp_u16);

        prefix = "rand_uint:32b:";
        benchmark::RegisterBenchmark(prefix + "arc4random", BM_rand_uint<uint32_t>, arc4random);
#if defined(__RDRND__)
        benchmark::RegisterBenchmark(prefix + "rdrand32", BM_rand_uint<uint32_t>, rdrand32);
#endif
#if defined(__RDSEED__)
        benchmark::RegisterBenchmark(prefix + "rdseed32", BM_rand_uint<uint32_t>, rdseed32);
#endif
        benchmark::RegisterBenchmark(prefix + "randp_u32", BM_rand_uint<uint32_t>, randp_u32);

        prefix = "rand_uint:64b:";
#if defined(__RDRND__)
        benchmark::RegisterBenchmark(prefix + "rdrand64", BM_rand_uint<uint64_t>, rdrand64);
#endif
#if defined(__RDSEED__)
        benchmark::RegisterBenchmark(prefix + "rdseed64", BM_rand_uint<uint64_t>, rdseed64);
#endif
        benchmark::RegisterBenchmark(prefix + "randp_u64", BM_rand_uint<uint64_t>, randp_u64);

        prefix = "rand_lt-one:32b:";
        benchmark::RegisterBenchmark(prefix + "arc4random_uniform", BM_rand_lt_one<uint32_t>, arc4random_uniform);
        benchmark::RegisterBenchmark(prefix + "randp_lt_u32", BM_rand_lt_one<uint32_t>, randp_lt_u32);

        prefix = "rand_lt-many:32b:";
        benchmark::RegisterBenchmark(prefix + "arc4random_uniform", BM_rand_lt_many<uint32_t>, arc4random_uniform);
        benchmark::RegisterBenchmark(prefix + "randp_lt_u32", BM_rand_lt_many<uint32_t>, randp_lt_u32);

        //for (size_t i = 16; i <= 256; i *= 2)
        for (size_t i = 1; i <= 256; i *= 2)
        {
            buf_size = i;
            prefix = "rand_bytes:" + std::to_string(i) + "B:";
            benchmark::RegisterBenchmark(prefix + "getentropy", BM_rand_bytes, getentropy_wrapper, buf_size);
            benchmark::RegisterBenchmark(prefix + "arc4random_buf", BM_rand_bytes, arc4random_buf, buf_size);
            benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_bytes, randp_bytes, buf_size);
        }

        //for (size_t i = 1; i <= 8; ++i)
        for (size_t i = 1; i <= 1; ++i)
        {
            buf_size = 4096 * i;
            prefix = "rand_bytes:" + std::to_string(i) + "pg:";
            benchmark::RegisterBenchmark(prefix + "arc4random_buf", BM_rand_bytes, arc4random_buf, buf_size);
            benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_bytes, randp_bytes, buf_size);
        }

        prefix = "rand_bytes_4GiB:";
        benchmark::RegisterBenchmark(prefix + "getentropy", BM_rand_bytes_4GiB, getentropy_wrapper)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "arc4random_buf", BM_rand_bytes_4GiB, arc4random_buf)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_bytes_4GiB, randp_bytes)->Unit(benchmark::kMillisecond);
    }
    else
    {
        prefix = "rand_uint:8b:";
        benchmark::RegisterBenchmark(prefix + "randp_u8", BM_rand_uint<uint8_t>, randp_u8)->Threads(num_threads);

        prefix = "rand_uint:16b:";
#if defined(__RDRND__)
        benchmark::RegisterBenchmark(prefix + "rdrand16", BM_rand_uint<uint16_t>, rdrand16)->Threads(num_threads);
#endif
#if defined(__RDSEED__)
        benchmark::RegisterBenchmark(prefix + "rdseed16", BM_rand_uint<uint16_t>, rdseed16)->Threads(num_threads);
#endif
        benchmark::RegisterBenchmark(prefix + "randp_u16", BM_rand_uint<uint16_t>, randp_u16)->Threads(num_threads);

        prefix = "rand_uint:32b:";
        benchmark::RegisterBenchmark(prefix + "arc4random", BM_rand_uint<uint32_t>, arc4random)->Threads(num_threads);
#if defined(__RDRND__)
        benchmark::RegisterBenchmark(prefix + "rdrand32", BM_rand_uint<uint32_t>, rdrand32)->Threads(num_threads);
#endif
#if defined(__RDSEED__)
        benchmark::RegisterBenchmark(prefix + "rdseed32", BM_rand_uint<uint32_t>, rdseed32)->Threads(num_threads);
#endif
        benchmark::RegisterBenchmark(prefix + "randp_u32", BM_rand_uint<uint32_t>, randp_u32)->Threads(num_threads);

        prefix = "rand_uint:64b:";
#if defined(__RDRND__)
        benchmark::RegisterBenchmark(prefix + "rdrand64", BM_rand_uint<uint64_t>, rdrand64)->Threads(num_threads);
#endif
#if defined(__RDSEED__)
        benchmark::RegisterBenchmark(prefix + "rdseed64", BM_rand_uint<uint64_t>, rdseed64)->Threads(num_threads);
#endif
        benchmark::RegisterBenchmark(prefix + "randp_u64", BM_rand_uint<uint64_t>, randp_u64)->Threads(num_threads);

        prefix = "rand_lt-one:32b:";
        benchmark::RegisterBenchmark(prefix + "arc4random_uniform", BM_rand_lt_one<uint32_t>, arc4random_uniform)->Threads(num_threads);
        benchmark::RegisterBenchmark(prefix + "randp_lt_u32", BM_rand_lt_one<uint32_t>, randp_lt_u32)->Threads(num_threads);

        prefix = "rand_lt-many:32b:";
        benchmark::RegisterBenchmark(prefix + "arc4random_uniform", BM_rand_lt_many<uint32_t>, arc4random_uniform)->Threads(num_threads);
        benchmark::RegisterBenchmark(prefix + "randp_lt_u32", BM_rand_lt_many<uint32_t>, randp_lt_u32)->Threads(num_threads);

        //for (size_t i = 16; i <= 256; i *= 2)
        for (size_t i = 1; i <= 256; i *= 2)
        {
            buf_size = i;
            prefix = "rand_bytes:" + std::to_string(i) + "B:";
            benchmark::RegisterBenchmark(prefix + "getentropy", BM_rand_bytes, getentropy_wrapper, buf_size)->Threads(num_threads);
            benchmark::RegisterBenchmark(prefix + "arc4random_buf", BM_rand_bytes, arc4random_buf, buf_size)->Threads(num_threads);
            benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_bytes, randp_bytes, buf_size)->Threads(num_threads);
        }

        //for (size_t i = 1; i <= 8; ++i)
        for (size_t i = 1; i <= 1; ++i)
        {
            buf_size = 4096 * i;
            prefix = "rand_bytes:" + std::to_string(i) + "pg:";
            benchmark::RegisterBenchmark(prefix + "arc4random_buf", BM_rand_bytes, arc4random_buf, buf_size)->Threads(num_threads);
            benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_bytes, randp_bytes, buf_size)->Threads(num_threads);
        }

        prefix = "rand_bytes_4GiB:";
        benchmark::RegisterBenchmark(prefix + "getentropy", BM_rand_bytes_4GiB, getentropy_wrapper)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "arc4random_buf", BM_rand_bytes_4GiB, arc4random_buf)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_bytes_4GiB, randp_bytes)->Threads(num_threads)->Unit(benchmark::kMillisecond);
    }

    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

    // }}}

    return 0;
}
