// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

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
	static_assert(std::has_single_bit(sizeof(buf)),
	              "buffer size must be a power of 2");

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
	benchmark::Initialize(&argc, argv);

	if (benchmark::ReportUnrecognizedArguments(argc, argv))
		return 1;

	// {{{ determine num_threads

	constexpr unsigned int min_threads = 1;
	const unsigned int max_threads = (std::thread::hardware_concurrency() != 0) ?
	                                 std::thread::hardware_concurrency() :
	                                 min_threads;
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
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<_1,1U<<31>", BM_rand_bytes_4GiB, randp_bytes< 1, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<_2,1U<<31>", BM_rand_bytes_4GiB, randp_bytes< 2, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<_3,1U<<31>", BM_rand_bytes_4GiB, randp_bytes< 3, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<_4,1U<<31>", BM_rand_bytes_4GiB, randp_bytes< 4, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<_5,1U<<31>", BM_rand_bytes_4GiB, randp_bytes< 5, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<_6,1U<<31>", BM_rand_bytes_4GiB, randp_bytes< 6, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<_7,1U<<31>", BM_rand_bytes_4GiB, randp_bytes< 7, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<_8,1U<<31>", BM_rand_bytes_4GiB, randp_bytes< 8, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<_9,1U<<31>", BM_rand_bytes_4GiB, randp_bytes< 9, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<10,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<10, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<11,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<11, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<12,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<12, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<13,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<13, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<14,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<14, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<15,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<15, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<16,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<16, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<17,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<17, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<18,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<18, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<19,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<19, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<20,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<20, 1U << 31>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<21,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<21, 1U << 31>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<22,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<22, 1U << 31>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<23,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<23, 1U << 31>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<24,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<24, 1U << 31>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<25,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<25, 1U << 31>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<26,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<26, 1U << 31>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<27,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<27, 1U << 31>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<28,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<28, 1U << 31>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<29,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<29, 1U << 31>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<30,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<30, 1U << 31>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<31,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<31, 1U << 31>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<32,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<32, 1U << 31>)->Unit(benchmark::kMillisecond);
	}
	else
	{
		prefix = "rand_bytes_4GiB:";
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<_1,1U<<31>", BM_rand_bytes_4GiB, randp_bytes< 1, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<_2,1U<<31>", BM_rand_bytes_4GiB, randp_bytes< 2, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<_3,1U<<31>", BM_rand_bytes_4GiB, randp_bytes< 3, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<_4,1U<<31>", BM_rand_bytes_4GiB, randp_bytes< 4, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<_5,1U<<31>", BM_rand_bytes_4GiB, randp_bytes< 5, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<_6,1U<<31>", BM_rand_bytes_4GiB, randp_bytes< 6, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<_7,1U<<31>", BM_rand_bytes_4GiB, randp_bytes< 7, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<_8,1U<<31>", BM_rand_bytes_4GiB, randp_bytes< 8, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<_9,1U<<31>", BM_rand_bytes_4GiB, randp_bytes< 9, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<10,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<10, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<11,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<11, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<12,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<12, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<13,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<13, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<14,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<14, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<15,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<15, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<16,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<16, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<17,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<17, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<18,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<18, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<19,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<19, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<20,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<20, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<21,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<21, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<22,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<22, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<23,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<23, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<24,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<24, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<25,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<25, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<26,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<26, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<27,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<27, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<28,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<28, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<29,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<29, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<30,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<30, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<31,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<31, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<32,1U<<31>", BM_rand_bytes_4GiB, randp_bytes<32, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
	}

	benchmark::RunSpecifiedBenchmarks();
	benchmark::Shutdown();

	// }}}

	return 0;
}
