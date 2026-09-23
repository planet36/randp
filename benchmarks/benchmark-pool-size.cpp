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

#include "get_num_threads.hpp"

#include <climits>
#include <format>
#include <utility>

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

    [num_threads]<int... SIZE_BYTES>(std::integer_sequence<int, SIZE_BYTES...>)
    {
        (benchmark::RegisterBenchmark(
             std::format("rand_bytes_4GiB:randp_bytes<{:_>3},MAX,m128i>", SIZE_BYTES),
             BM_rand_bytes_4GiB,
             randp_bytes<SIZE_BYTES, INT_MAX, __m128i>)
             ->Threads(num_threads)
             ->Unit(benchmark::kMillisecond),
         ...);

        (benchmark::RegisterBenchmark(
             std::format("rand_bytes_4GiB:randp_bytes<{:_>3},MAX,m256i>", SIZE_BYTES),
             BM_rand_bytes_4GiB,
             randp_bytes<SIZE_BYTES, INT_MAX, __m256i>)
             ->Threads(num_threads)
             ->Unit(benchmark::kMillisecond),
         ...);
    }(std::integer_sequence<int, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512>{});

    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

    // }}}

    return 0;
}
