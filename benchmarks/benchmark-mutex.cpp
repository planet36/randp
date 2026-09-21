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

#include "parse_int.hpp"

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

    // {{{ determine num_threads

    constexpr int min_threads = 1;
    const auto hw_threads = static_cast<int>(std::thread::hardware_concurrency());
    const auto max_threads = std::max(min_threads, hw_threads);

    // NUM_THREADS=0 means max_threads
    int num_threads = min_threads;

    try
    {
        num_threads = parse_env_int("NUM_THREADS", 0, max_threads, min_threads);
    }
    catch (const std::exception& ex)
    {
        (void)std::fflush(stdout);
        errx(EXIT_FAILURE, "%s", ex.what());
    }

    if (num_threads == 0)
        num_threads = max_threads;

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
    size_t buf_size = 0;

    if (num_threads == 1)
    {
        for (int i = 1; i <= 256; i *= 2)
        {
            buf_size = i;
            prefix = "rand_bytes:" + std::to_string(i) + "B:";
            benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_bytes, randp_bytes, buf_size);
            benchmark::RegisterBenchmark(prefix + "randp_bytes_MUTEX", BM_rand_bytes, randp_bytes_MUTEX, buf_size);
        }

        //for (int i = 1; i <= 8; ++i)
        for (int i = 1; i <= 1; ++i)
        {
            buf_size = 4096 * i;
            prefix = "rand_bytes:" + std::to_string(i) + "pg:";
            benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_bytes, randp_bytes, buf_size);
            benchmark::RegisterBenchmark(prefix + "randp_bytes_MUTEX", BM_rand_bytes, randp_bytes_MUTEX, buf_size);
        }

        prefix = "rand_bytes_4GiB:";
        benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_bytes_4GiB, randp_bytes)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes_MUTEX", BM_rand_bytes_4GiB, randp_bytes_MUTEX)->Unit(benchmark::kMillisecond);
    }
    else
    {
        for (int i = 1; i <= 256; i *= 2)
        {
            buf_size = i;
            prefix = "rand_bytes:" + std::to_string(i) + "B:";
            benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_bytes, randp_bytes, buf_size)->Threads(num_threads);
            benchmark::RegisterBenchmark(prefix + "randp_bytes_MUTEX", BM_rand_bytes, randp_bytes_MUTEX, buf_size)->Threads(num_threads);
        }

        //for (int i = 1; i <= 8; ++i)
        for (int i = 1; i <= 1; ++i)
        {
            buf_size = 4096 * i;
            prefix = "rand_bytes:" + std::to_string(i) + "pg:";
            benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_bytes, randp_bytes, buf_size)->Threads(num_threads);
            benchmark::RegisterBenchmark(prefix + "randp_bytes_MUTEX", BM_rand_bytes, randp_bytes_MUTEX, buf_size)->Threads(num_threads);
        }

        prefix = "rand_bytes_4GiB:";
        benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_bytes_4GiB, randp_bytes)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes_MUTEX", BM_rand_bytes_4GiB, randp_bytes_MUTEX)->Threads(num_threads)->Unit(benchmark::kMillisecond);
    }

    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

    // }}}

    return 0;
}
