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

    for (auto _ : BM_state) // NOLINT(clang-analyzer-deadcode.DeadStores)
    {
        // This code gets timed

        for (size_t i = 0; i < (1UL << 32) / sizeof(buf); ++i)
        {
            fn(buf, sizeof(buf));
        }
    }
}

#include "parse_int.hpp"

#include <algorithm>
#include <climits>
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
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<_1,MAX>", BM_rand_bytes_4GiB, randp_bytes< 1, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<_2,MAX>", BM_rand_bytes_4GiB, randp_bytes< 2, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<_3,MAX>", BM_rand_bytes_4GiB, randp_bytes< 3, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<_4,MAX>", BM_rand_bytes_4GiB, randp_bytes< 4, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<_5,MAX>", BM_rand_bytes_4GiB, randp_bytes< 5, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<_6,MAX>", BM_rand_bytes_4GiB, randp_bytes< 6, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<_7,MAX>", BM_rand_bytes_4GiB, randp_bytes< 7, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<_8,MAX>", BM_rand_bytes_4GiB, randp_bytes< 8, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<_9,MAX>", BM_rand_bytes_4GiB, randp_bytes< 9, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<10,MAX>", BM_rand_bytes_4GiB, randp_bytes<10, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<11,MAX>", BM_rand_bytes_4GiB, randp_bytes<11, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<12,MAX>", BM_rand_bytes_4GiB, randp_bytes<12, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<13,MAX>", BM_rand_bytes_4GiB, randp_bytes<13, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<14,MAX>", BM_rand_bytes_4GiB, randp_bytes<14, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<15,MAX>", BM_rand_bytes_4GiB, randp_bytes<15, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<16,MAX>", BM_rand_bytes_4GiB, randp_bytes<16, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<17,MAX>", BM_rand_bytes_4GiB, randp_bytes<17, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<18,MAX>", BM_rand_bytes_4GiB, randp_bytes<18, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<19,MAX>", BM_rand_bytes_4GiB, randp_bytes<19, INT_MAX>)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<20,MAX>", BM_rand_bytes_4GiB, randp_bytes<20, INT_MAX>)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<21,MAX>", BM_rand_bytes_4GiB, randp_bytes<21, INT_MAX>)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<22,MAX>", BM_rand_bytes_4GiB, randp_bytes<22, INT_MAX>)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<23,MAX>", BM_rand_bytes_4GiB, randp_bytes<23, INT_MAX>)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<24,MAX>", BM_rand_bytes_4GiB, randp_bytes<24, INT_MAX>)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<25,MAX>", BM_rand_bytes_4GiB, randp_bytes<25, INT_MAX>)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<26,MAX>", BM_rand_bytes_4GiB, randp_bytes<26, INT_MAX>)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<27,MAX>", BM_rand_bytes_4GiB, randp_bytes<27, INT_MAX>)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<28,MAX>", BM_rand_bytes_4GiB, randp_bytes<28, INT_MAX>)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<29,MAX>", BM_rand_bytes_4GiB, randp_bytes<29, INT_MAX>)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<30,MAX>", BM_rand_bytes_4GiB, randp_bytes<30, INT_MAX>)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<31,MAX>", BM_rand_bytes_4GiB, randp_bytes<31, INT_MAX>)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<32,MAX>", BM_rand_bytes_4GiB, randp_bytes<32, INT_MAX>)->Unit(benchmark::kMillisecond);
    }
    else
    {
        prefix = "rand_bytes_4GiB:";
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<_1,MAX>", BM_rand_bytes_4GiB, randp_bytes< 1, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<_2,MAX>", BM_rand_bytes_4GiB, randp_bytes< 2, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<_3,MAX>", BM_rand_bytes_4GiB, randp_bytes< 3, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<_4,MAX>", BM_rand_bytes_4GiB, randp_bytes< 4, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<_5,MAX>", BM_rand_bytes_4GiB, randp_bytes< 5, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<_6,MAX>", BM_rand_bytes_4GiB, randp_bytes< 6, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<_7,MAX>", BM_rand_bytes_4GiB, randp_bytes< 7, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<_8,MAX>", BM_rand_bytes_4GiB, randp_bytes< 8, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<_9,MAX>", BM_rand_bytes_4GiB, randp_bytes< 9, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<10,MAX>", BM_rand_bytes_4GiB, randp_bytes<10, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<11,MAX>", BM_rand_bytes_4GiB, randp_bytes<11, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<12,MAX>", BM_rand_bytes_4GiB, randp_bytes<12, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<13,MAX>", BM_rand_bytes_4GiB, randp_bytes<13, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<14,MAX>", BM_rand_bytes_4GiB, randp_bytes<14, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<15,MAX>", BM_rand_bytes_4GiB, randp_bytes<15, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<16,MAX>", BM_rand_bytes_4GiB, randp_bytes<16, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<17,MAX>", BM_rand_bytes_4GiB, randp_bytes<17, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<18,MAX>", BM_rand_bytes_4GiB, randp_bytes<18, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<19,MAX>", BM_rand_bytes_4GiB, randp_bytes<19, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        benchmark::RegisterBenchmark(prefix + "randp_bytes<20,MAX>", BM_rand_bytes_4GiB, randp_bytes<20, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<21,MAX>", BM_rand_bytes_4GiB, randp_bytes<21, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<22,MAX>", BM_rand_bytes_4GiB, randp_bytes<22, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<23,MAX>", BM_rand_bytes_4GiB, randp_bytes<23, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<24,MAX>", BM_rand_bytes_4GiB, randp_bytes<24, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<25,MAX>", BM_rand_bytes_4GiB, randp_bytes<25, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<26,MAX>", BM_rand_bytes_4GiB, randp_bytes<26, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<27,MAX>", BM_rand_bytes_4GiB, randp_bytes<27, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<28,MAX>", BM_rand_bytes_4GiB, randp_bytes<28, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<29,MAX>", BM_rand_bytes_4GiB, randp_bytes<29, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<30,MAX>", BM_rand_bytes_4GiB, randp_bytes<30, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<31,MAX>", BM_rand_bytes_4GiB, randp_bytes<31, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
        //benchmark::RegisterBenchmark(prefix + "randp_bytes<32,MAX>", BM_rand_bytes_4GiB, randp_bytes<32, INT_MAX>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
    }

    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

    // }}}

    return 0;
}
