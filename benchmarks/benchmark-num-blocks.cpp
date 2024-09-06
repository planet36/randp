// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

#include "randp.hpp"

// https://github.com/google/benchmark

#include <benchmark/benchmark.h>
#include <bit>
#include <concepts>
#include <functional>

void
BM_rand_buf_4GiB(benchmark::State& state,
                 const std::function<void(uint8_t*, size_t)>& fn)
{
	// Perform setup here

	uint8_t buf[1U << 8];
	static_assert(sizeof(buf) <= 256,
	              "getentropy will fail if more than 256 bytes are requested");
	static_assert(std::has_single_bit(sizeof(buf)),
	              "buffer size must be a power of 2");

	for (auto _ : state)
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
	// copied from benchmark.h
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
		prefix = "rand_buf_4GiB:";
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x01,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x01, 1U << 31>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x02,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x02, 1U << 31>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x03,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x03, 1U << 31>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x04,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x04, 1U << 31>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x05,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x05, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x06,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x06, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x07,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x07, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x08,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x08, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x09,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x09, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x0a,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x0a, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x0b,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x0b, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x0c,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x0c, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x0d,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x0d, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x0e,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x0e, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x0f,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x0f, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x10, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x11,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x11, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x12,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x12, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x13,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x13, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x14,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x14, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x15,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x15, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x16,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x16, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x17,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x17, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x18,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x18, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x19,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x19, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x1a,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x1a, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x1b,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x1b, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x1c,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x1c, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x1d,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x1d, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x1e,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x1e, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x1f,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x1f, 1U << 31>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x20,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x20, 1U << 31>)->Unit(benchmark::kMillisecond);
	}
	else
	{
		prefix = "rand_buf_4GiB:";
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x01,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x01, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x02,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x02, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x03,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x03, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x04,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x04, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x05,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x05, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x06,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x06, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x07,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x07, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x08,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x08, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x09,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x09, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x0a,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x0a, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x0b,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x0b, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x0c,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x0c, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x0d,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x0d, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x0e,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x0e, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x0f,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x0f, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x10, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x11,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x11, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x12,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x12, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x13,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x13, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x14,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x14, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x15,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x15, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x16,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x16, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x17,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x17, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x18,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x18, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x19,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x19, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x1a,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x1a, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x1b,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x1b, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x1c,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x1c, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x1d,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x1d, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x1e,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x1e, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x1f,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x1f, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x20,1U<<31>", BM_rand_buf_4GiB, randp_bytes<0x20, 1U << 31>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
	}

	benchmark::RunSpecifiedBenchmarks();
	benchmark::Shutdown();

	// }}}

	return 0;
}
