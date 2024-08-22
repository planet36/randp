// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

#include "randp.hpp"

// https://github.com/google/benchmark

#include <benchmark/benchmark.h>
#include <concepts>
#include <functional>

void
BM_rand_buf(benchmark::State& state,
            const std::function<void(uint8_t*, size_t)>& fn,
            const size_t buf_size)
{
	// Perform setup here

	uint8_t* buf = new uint8_t[buf_size];

	for (auto _ : state)
	{
		// This code gets timed

		fn(buf, buf_size);
	}

	delete[] buf;

	state.SetBytesProcessed(state.iterations() * buf_size);
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
	size_t buf_size;

	if (num_threads == 1)
	{
		for (size_t i = 1; i <= 256; i *= 2)
		{
			buf_size = i;
			prefix = "rand_buf:" + std::to_string(i) + "B:";
			benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_buf, randp_bytes, buf_size);
			benchmark::RegisterBenchmark(prefix + "randp_bytes_MUTEX", BM_rand_buf, randp_bytes_MUTEX, buf_size);
		}

		//for (size_t i = 1; i <= 8; ++i)
		for (size_t i = 1; i <= 1; ++i)
		{
			buf_size = 4096 * i;
			prefix = "rand_buf:" + std::to_string(i) + "pg:";
			benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_buf, randp_bytes, buf_size);
			benchmark::RegisterBenchmark(prefix + "randp_bytes_MUTEX", BM_rand_buf, randp_bytes_MUTEX, buf_size);
		}
	}
	else
	{
		for (size_t i = 1; i <= 256; i *= 2)
		{
			buf_size = i;
			prefix = "rand_buf:" + std::to_string(i) + "B:";
			benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_buf, randp_bytes, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "randp_bytes_MUTEX", BM_rand_buf, randp_bytes_MUTEX, buf_size)->Threads(num_threads);
		}

		//for (size_t i = 1; i <= 8; ++i)
		for (size_t i = 1; i <= 1; ++i)
		{
			buf_size = 4096 * i;
			prefix = "rand_buf:" + std::to_string(i) + "pg:";
			benchmark::RegisterBenchmark(prefix + "randp_bytes", BM_rand_buf, randp_bytes, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "randp_bytes_MUTEX", BM_rand_buf, randp_bytes_MUTEX, buf_size)->Threads(num_threads);
		}
	}

	benchmark::RunSpecifiedBenchmarks();
	benchmark::Shutdown();

	// }}}

	return 0;
}
