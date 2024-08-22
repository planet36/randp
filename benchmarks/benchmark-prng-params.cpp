// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

#include "randp.hpp"

// https://github.com/google/benchmark

#include <benchmark/benchmark.h>
#include <concepts>
#include <functional>

void
BM_rand_buf_1GiB(benchmark::State& state,
                 const std::function<void(uint8_t*, size_t)>& fn)
{
	// Perform setup here

	uint8_t buf[1U << 8];

	for (auto _ : state)
	{
		// This code gets timed

		// (2**30) / (2**8) == 2**22
		for (size_t i = 0; i < (1U << 22); ++i)
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
		prefix = "rand_buf_1GiB:";

		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,1,1>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 1, 1>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,1,2>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 1, 2>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,1,3>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 1, 3>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,1,4>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 1, 4>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,1,5>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 1, 5>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,1,6>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 1, 6>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,1,7>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 1, 7>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,1,8>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 1, 8>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,1,9>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 1, 9>)->Unit(benchmark::kMillisecond);

		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,2,1>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 2, 1>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,2,2>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 2, 2>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,2,3>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 2, 3>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,2,4>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 2, 4>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,2,5>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 2, 5>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,2,6>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 2, 6>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,2,7>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 2, 7>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,2,8>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 2, 8>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,2,9>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 2, 9>)->Unit(benchmark::kMillisecond);

		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,3,1>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 3, 1>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,3,2>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 3, 2>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,3,3>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 3, 3>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,3,4>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 3, 4>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,3,5>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 3, 5>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,3,6>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 3, 6>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,3,7>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 3, 7>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,3,8>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 3, 8>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,3,9>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 3, 9>)->Unit(benchmark::kMillisecond);

		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,1,1>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 1, 1>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,1,2>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 1, 2>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,1,3>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 1, 3>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,1,4>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 1, 4>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,1,5>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 1, 5>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,1,6>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 1, 6>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,1,7>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 1, 7>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,1,8>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 1, 8>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,1,9>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 1, 9>)->Unit(benchmark::kMillisecond);

		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,2,1>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 2, 1>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,2,2>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 2, 2>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,2,3>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 2, 3>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,2,4>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 2, 4>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,2,5>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 2, 5>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,2,6>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 2, 6>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,2,7>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 2, 7>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,2,8>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 2, 8>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,2,9>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 2, 9>)->Unit(benchmark::kMillisecond);

		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,3,1>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 3, 1>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,3,2>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 3, 2>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,3,3>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 3, 3>)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,3,4>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 3, 4>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,3,5>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 3, 5>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,3,6>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 3, 6>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,3,7>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 3, 7>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,3,8>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 3, 8>)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,3,9>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 3, 9>)->Unit(benchmark::kMillisecond);
	}
	else
	{
		prefix = "rand_buf_1GiB:";

		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,1,1>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 1, 1>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,1,2>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 1, 2>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,1,3>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 1, 3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,1,4>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 1, 4>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,1,5>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 1, 5>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,1,6>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 1, 6>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,1,7>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 1, 7>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,1,8>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 1, 8>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,1,9>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 1, 9>)->Threads(num_threads)->Unit(benchmark::kMillisecond);

		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,2,1>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 2, 1>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,2,2>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 2, 2>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,2,3>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 2, 3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,2,4>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 2, 4>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,2,5>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 2, 5>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,2,6>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 2, 6>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,2,7>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 2, 7>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,2,8>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 2, 8>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,2,9>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 2, 9>)->Threads(num_threads)->Unit(benchmark::kMillisecond);

		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,3,1>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 3, 1>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,3,2>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 3, 2>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,3,3>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 3, 3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,3,4>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 3, 4>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,3,5>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 3, 5>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,3,6>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 3, 6>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,3,7>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 3, 7>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,3,8>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 3, 8>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,enc,3,9>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, true, 3, 9>)->Threads(num_threads)->Unit(benchmark::kMillisecond);

		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,1,1>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 1, 1>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,1,2>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 1, 2>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,1,3>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 1, 3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,1,4>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 1, 4>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,1,5>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 1, 5>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,1,6>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 1, 6>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,1,7>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 1, 7>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,1,8>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 1, 8>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,1,9>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 1, 9>)->Threads(num_threads)->Unit(benchmark::kMillisecond);

		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,2,1>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 2, 1>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,2,2>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 2, 2>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,2,3>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 2, 3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,2,4>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 2, 4>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,2,5>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 2, 5>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,2,6>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 2, 6>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,2,7>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 2, 7>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,2,8>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 2, 8>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,2,9>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 2, 9>)->Threads(num_threads)->Unit(benchmark::kMillisecond);

		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,3,1>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 3, 1>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,3,2>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 3, 2>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,3,3>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 3, 3>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,3,4>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 3, 4>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,3,5>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 3, 5>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,3,6>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 3, 6>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,3,7>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 3, 7>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,3,8>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 3, 8>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
		//benchmark::RegisterBenchmark(prefix + "randp_bytes<0x10,1U<<10,dec,3,9>", BM_rand_buf_1GiB, randp_bytes<0x10, 1U << 10, false, 3, 9>)->Threads(num_threads)->Unit(benchmark::kMillisecond);
	}

	benchmark::RunSpecifiedBenchmarks();
	benchmark::Shutdown();

	// }}}

	return 0;
}
