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

    benchmark::RegisterBenchmark("rand_bytes_4GiB:randp_bytes<def,def,enc,dm,1,3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_POOL_SIZE_BYTES, DEFAULT_RANDP_RESEED_INTERVAL, true, true, 1, 3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
    benchmark::RegisterBenchmark("rand_bytes_4GiB:randp_bytes<def,def,enc,dm,1,4>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_POOL_SIZE_BYTES, DEFAULT_RANDP_RESEED_INTERVAL, true, true, 1, 4>)->Threads(num_threads)->Unit(benchmark::kMillisecond);

    benchmark::RegisterBenchmark("rand_bytes_4GiB:randp_bytes<def,def,enc,dm,2,2>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_POOL_SIZE_BYTES, DEFAULT_RANDP_RESEED_INTERVAL, true, true, 2, 2>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
    benchmark::RegisterBenchmark("rand_bytes_4GiB:randp_bytes<def,def,enc,dm,2,3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_POOL_SIZE_BYTES, DEFAULT_RANDP_RESEED_INTERVAL, true, true, 2, 3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
    benchmark::RegisterBenchmark("rand_bytes_4GiB:randp_bytes<def,def,enc,dm,2,4>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_POOL_SIZE_BYTES, DEFAULT_RANDP_RESEED_INTERVAL, true, true, 2, 4>)->Threads(num_threads)->Unit(benchmark::kMillisecond);

    benchmark::RegisterBenchmark("rand_bytes_4GiB:randp_bytes<def,def,enc,dm,3,1>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_POOL_SIZE_BYTES, DEFAULT_RANDP_RESEED_INTERVAL, true, true, 3, 1>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
    benchmark::RegisterBenchmark("rand_bytes_4GiB:randp_bytes<def,def,enc,dm,3,2>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_POOL_SIZE_BYTES, DEFAULT_RANDP_RESEED_INTERVAL, true, true, 3, 2>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
    benchmark::RegisterBenchmark("rand_bytes_4GiB:randp_bytes<def,def,enc,dm,3,3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_POOL_SIZE_BYTES, DEFAULT_RANDP_RESEED_INTERVAL, true, true, 3, 3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);


    benchmark::RegisterBenchmark("rand_bytes_4GiB:randp_bytes<def,def,enc,no-dm,1,3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_POOL_SIZE_BYTES, DEFAULT_RANDP_RESEED_INTERVAL, true, false, 1, 3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
    benchmark::RegisterBenchmark("rand_bytes_4GiB:randp_bytes<def,def,enc,no-dm,1,4>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_POOL_SIZE_BYTES, DEFAULT_RANDP_RESEED_INTERVAL, true, false, 1, 4>)->Threads(num_threads)->Unit(benchmark::kMillisecond);

    benchmark::RegisterBenchmark("rand_bytes_4GiB:randp_bytes<def,def,enc,no-dm,2,2>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_POOL_SIZE_BYTES, DEFAULT_RANDP_RESEED_INTERVAL, true, false, 2, 2>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
    benchmark::RegisterBenchmark("rand_bytes_4GiB:randp_bytes<def,def,enc,no-dm,2,3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_POOL_SIZE_BYTES, DEFAULT_RANDP_RESEED_INTERVAL, true, false, 2, 3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
    benchmark::RegisterBenchmark("rand_bytes_4GiB:randp_bytes<def,def,enc,no-dm,2,4>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_POOL_SIZE_BYTES, DEFAULT_RANDP_RESEED_INTERVAL, true, false, 2, 4>)->Threads(num_threads)->Unit(benchmark::kMillisecond);

    benchmark::RegisterBenchmark("rand_bytes_4GiB:randp_bytes<def,def,enc,no-dm,3,1>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_POOL_SIZE_BYTES, DEFAULT_RANDP_RESEED_INTERVAL, true, false, 3, 1>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
    benchmark::RegisterBenchmark("rand_bytes_4GiB:randp_bytes<def,def,enc,no-dm,3,2>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_POOL_SIZE_BYTES, DEFAULT_RANDP_RESEED_INTERVAL, true, false, 3, 2>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
    benchmark::RegisterBenchmark("rand_bytes_4GiB:randp_bytes<def,def,enc,no-dm,3,3>", BM_rand_bytes_4GiB, randp_bytes<DEFAULT_RANDP_POOL_SIZE_BYTES, DEFAULT_RANDP_RESEED_INTERVAL, true, false, 3, 3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);

    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

    // }}}

    return 0;
}
