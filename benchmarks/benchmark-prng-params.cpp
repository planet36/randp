// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

#include "randp.hpp"

// https://github.com/google/benchmark

#include <benchmark/benchmark.h>
#include <bit>
#include <concepts>
#include <functional>

void
BM_rand_bytes_4GiB(benchmark::State& BM_state,
                   const std::function<void(uint8_t*, size_t)>& fn)
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
        num_threads = max_threads;
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

    if (num_threads == 1)
    {
        prefix = "rand_bytes_4GiB:";

        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,dm,1,3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, true, 1, 3>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,dm,1,4>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, true, 1, 4>)->Unit(benchmark::kMillisecond);

        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,dm,2,2>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, true, 2, 2>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,dm,2,3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, true, 2, 3>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,dm,2,4>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, true, 2, 4>)->Unit(benchmark::kMillisecond);

        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,dm,3,1>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, true, 3, 1>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,dm,3,2>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, true, 3, 2>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,dm,3,3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, true, 3, 3>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,dm,3,4>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, true, 3, 4>)->Unit(benchmark::kMillisecond);


        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,no-dm,1,3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, false, 1, 3>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,no-dm,1,4>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, false, 1, 4>)->Unit(benchmark::kMillisecond);

        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,no-dm,2,2>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, false, 2, 2>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,no-dm,2,3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, false, 2, 3>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,no-dm,2,4>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, false, 2, 4>)->Unit(benchmark::kMillisecond);

        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,no-dm,3,1>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, false, 3, 1>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,no-dm,3,2>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, false, 3, 2>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,no-dm,3,3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, false, 3, 3>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,no-dm,3,4>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, false, 3, 4>)->Unit(benchmark::kMillisecond);
    }
    else
    {
        prefix = "rand_bytes_4GiB:";

        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,dm,1,3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, true, 1, 3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,dm,1,4>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, true, 1, 4>)->Threads(num_threads)->Unit(benchmark::kMillisecond);

        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,dm,2,2>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, true, 2, 2>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,dm,2,3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, true, 2, 3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,dm,2,4>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, true, 2, 4>)->Threads(num_threads)->Unit(benchmark::kMillisecond);

        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,dm,3,1>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, true, 3, 1>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,dm,3,2>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, true, 3, 2>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,dm,3,3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, true, 3, 3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,dm,3,4>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, true, 3, 4>)->Threads(num_threads)->Unit(benchmark::kMillisecond);


        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,no-dm,1,3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, false, 1, 3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,no-dm,1,4>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, false, 1, 4>)->Threads(num_threads)->Unit(benchmark::kMillisecond);

        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,no-dm,2,2>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, false, 2, 2>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,no-dm,2,3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, false, 2, 3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,no-dm,2,4>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, false, 2, 4>)->Threads(num_threads)->Unit(benchmark::kMillisecond);

        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,no-dm,3,1>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, false, 3, 1>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,no-dm,3,2>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, false, 3, 2>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,no-dm,3,3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, false, 3, 3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<def,def,enc,no-dm,3,4>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_NUM_BLOCKS, DEFAULT_RANDP_RESEED_COUNTDOWN_MIN, true, false, 3, 4>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
    }

    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

    // }}}

    return 0;
}
