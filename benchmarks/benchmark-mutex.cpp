// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

#include "randp.hpp"

#include <benchmark/benchmark.h> // https://github.com/google/benchmark
#include <bit>

using func_t = void (&)(void*, size_t);

void
BM_rand_bytes(benchmark::State& BM_state, func_t& fn, const size_t buf_size)
{
    // Perform setup here

    auto* buf = new uint8_t[buf_size];

    for (auto _ : BM_state) // NOLINT(clang-analyzer-deadcode.DeadStores)
    {
        // This code gets timed

        fn(buf, buf_size);
    }

    delete[] buf;

    // Counters are summed across threads.  kAvgThreads makes this the per-thread rate.
    BM_state.counters["bytes_per_second"] =
        benchmark::Counter(static_cast<double>(BM_state.iterations() * buf_size),
                           benchmark::Counter::kAvgThreadsRate, benchmark::Counter::kIs1024);
}

void
BM_rand_bytes_4GiB(benchmark::State& BM_state, func_t& fn)
{
    // Perform setup here

    uint8_t buf[1 << 8];
    static_assert(sizeof(buf) <= 256,
                  "getentropy will fail if more than 256 bytes are requested");
    static_assert(std::has_single_bit(sizeof(buf)), "buffer size must be a power of 2");

    constexpr size_t num_iterations = (1UL << 32) / sizeof(buf); // 4 GiB

    for (auto _ : BM_state) // NOLINT(clang-analyzer-deadcode.DeadStores)
    {
        // This code gets timed

        for (size_t i = 0; i < num_iterations; ++i)
        {
            fn(buf, sizeof(buf));
        }
    }
}

#include "get_num_threads.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <err.h>
#include <stdexcept>
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

    const int num_threads = get_num_threads();

    // {{{ accuracy testing

    // }}}

    // {{{ speed

    std::string prefix;

    for (int i = 1; i <= 256; i *= 2)
    {
        const size_t buf_size = i;
        prefix = "rand_bytes:" + std::to_string(i) + "B:";
        benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_bytes, randp_bytes, buf_size)->Threads(num_threads);
        benchmark::RegisterBenchmark(prefix + "randp_bytes_MUTEX", BM_rand_bytes, randp_bytes_MUTEX, buf_size)->Threads(num_threads);
    }

    //for (int i = 1; i <= 8; ++i)
    for (int i = 1; i <= 1; ++i)
    {
        const size_t buf_size = 4096 * i;
        prefix = "rand_bytes:" + std::to_string(i) + "pg:";
        benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_bytes, randp_bytes, buf_size)->Threads(num_threads);
        benchmark::RegisterBenchmark(prefix + "randp_bytes_MUTEX", BM_rand_bytes, randp_bytes_MUTEX, buf_size)->Threads(num_threads);
    }

    prefix = "rand_bytes_4GiB:";
    benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_bytes_4GiB, randp_bytes)->Threads(num_threads)->Unit(benchmark::kMillisecond);
    benchmark::RegisterBenchmark(prefix + "randp_bytes_MUTEX", BM_rand_bytes_4GiB, randp_bytes_MUTEX)->Threads(num_threads)->Unit(benchmark::kMillisecond);

    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

    // }}}

    return 0;
}
