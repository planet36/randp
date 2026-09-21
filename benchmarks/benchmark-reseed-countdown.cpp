// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

#include "randp.hpp"

#include <benchmark/benchmark.h> // https://github.com/google/benchmark
#include <bit>

using func_t = void (&)(void*, size_t);

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

    if (num_threads == 1)
    {
        prefix = "rand_bytes_4GiB:";
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<_1>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 <<  1>)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<_2>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 <<  2>)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<_3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 <<  3>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<_4>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 <<  4>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<_5>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 <<  5>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<_6>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 <<  6>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<_7>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 <<  7>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<_8>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 <<  8>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<_9>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 <<  9>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<10>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 << 10>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<11>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 << 11>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<12>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 << 12>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<13>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 << 13>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<14>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 << 14>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<15>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 << 15>)->Unit(benchmark::kMillisecond);
    }
    else
    {
        prefix = "rand_bytes_4GiB:";
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<_1>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 <<  1>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<_2>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 <<  2>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<_3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 <<  3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<_4>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 <<  4>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<_5>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 <<  5>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<_6>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 <<  6>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<_7>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 <<  7>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<_8>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 <<  8>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<_9>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 <<  9>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<10>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 << 10>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<11>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 << 11>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<12>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 << 12>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<13>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 << 13>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<14>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 << 14>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,1<<15>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, 1 << 15>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
    }

    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

    // }}}

    return 0;
}
