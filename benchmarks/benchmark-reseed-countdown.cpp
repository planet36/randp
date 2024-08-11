// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

#include "arp.hpp"

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
		for (size_t i = 16; i <= 256; i *= 2)
		//for (size_t i = 1; i <= 256; i *= 2)
		{
			buf_size = i;
			prefix = "rand_buf:" + std::to_string(i) + "B:";
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_1>", BM_rand_buf, arp_copy_to<0x10,1U <<  1>, buf_size);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_2>", BM_rand_buf, arp_copy_to<0x10,1U <<  2>, buf_size);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_3>", BM_rand_buf, arp_copy_to<0x10,1U <<  3>, buf_size);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_4>", BM_rand_buf, arp_copy_to<0x10,1U <<  4>, buf_size);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_5>", BM_rand_buf, arp_copy_to<0x10,1U <<  5>, buf_size);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_6>", BM_rand_buf, arp_copy_to<0x10,1U <<  6>, buf_size);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_7>", BM_rand_buf, arp_copy_to<0x10,1U <<  7>, buf_size);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_8>", BM_rand_buf, arp_copy_to<0x10,1U <<  8>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_9>", BM_rand_buf, arp_copy_to<0x10,1U <<  9>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<10>", BM_rand_buf, arp_copy_to<0x10,1U << 10>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<11>", BM_rand_buf, arp_copy_to<0x10,1U << 11>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<12>", BM_rand_buf, arp_copy_to<0x10,1U << 12>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<13>", BM_rand_buf, arp_copy_to<0x10,1U << 13>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<14>", BM_rand_buf, arp_copy_to<0x10,1U << 14>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<15>", BM_rand_buf, arp_copy_to<0x10,1U << 15>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<16>", BM_rand_buf, arp_copy_to<0x10,1U << 16>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<17>", BM_rand_buf, arp_copy_to<0x10,1U << 17>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<18>", BM_rand_buf, arp_copy_to<0x10,1U << 18>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<19>", BM_rand_buf, arp_copy_to<0x10,1U << 19>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<20>", BM_rand_buf, arp_copy_to<0x10,1U << 20>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<21>", BM_rand_buf, arp_copy_to<0x10,1U << 21>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<22>", BM_rand_buf, arp_copy_to<0x10,1U << 22>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<23>", BM_rand_buf, arp_copy_to<0x10,1U << 23>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<24>", BM_rand_buf, arp_copy_to<0x10,1U << 24>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<25>", BM_rand_buf, arp_copy_to<0x10,1U << 25>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<26>", BM_rand_buf, arp_copy_to<0x10,1U << 26>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<27>", BM_rand_buf, arp_copy_to<0x10,1U << 27>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<28>", BM_rand_buf, arp_copy_to<0x10,1U << 28>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<29>", BM_rand_buf, arp_copy_to<0x10,1U << 29>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<30>", BM_rand_buf, arp_copy_to<0x10,1U << 30>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<31>", BM_rand_buf, arp_copy_to<0x10,1U << 31>, buf_size);
		}

		//for (size_t i = 1; i <= 8; ++i)
		for (size_t i = 1; i <= 1; ++i)
		{
			buf_size = 4096 * i;
			prefix = "rand_buf:" + std::to_string(i) + "pg:";
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<16>", BM_rand_buf, arp_copy_to<16>, buf_size);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_1>", BM_rand_buf, arp_copy_to<0x10,1U <<  1>, buf_size);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_2>", BM_rand_buf, arp_copy_to<0x10,1U <<  2>, buf_size);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_3>", BM_rand_buf, arp_copy_to<0x10,1U <<  3>, buf_size);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_4>", BM_rand_buf, arp_copy_to<0x10,1U <<  4>, buf_size);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_5>", BM_rand_buf, arp_copy_to<0x10,1U <<  5>, buf_size);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_6>", BM_rand_buf, arp_copy_to<0x10,1U <<  6>, buf_size);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_7>", BM_rand_buf, arp_copy_to<0x10,1U <<  7>, buf_size);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_8>", BM_rand_buf, arp_copy_to<0x10,1U <<  8>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_9>", BM_rand_buf, arp_copy_to<0x10,1U <<  9>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<10>", BM_rand_buf, arp_copy_to<0x10,1U << 10>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<11>", BM_rand_buf, arp_copy_to<0x10,1U << 11>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<12>", BM_rand_buf, arp_copy_to<0x10,1U << 12>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<13>", BM_rand_buf, arp_copy_to<0x10,1U << 13>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<14>", BM_rand_buf, arp_copy_to<0x10,1U << 14>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<15>", BM_rand_buf, arp_copy_to<0x10,1U << 15>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<16>", BM_rand_buf, arp_copy_to<0x10,1U << 16>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<17>", BM_rand_buf, arp_copy_to<0x10,1U << 17>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<18>", BM_rand_buf, arp_copy_to<0x10,1U << 18>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<19>", BM_rand_buf, arp_copy_to<0x10,1U << 19>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<20>", BM_rand_buf, arp_copy_to<0x10,1U << 20>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<21>", BM_rand_buf, arp_copy_to<0x10,1U << 21>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<22>", BM_rand_buf, arp_copy_to<0x10,1U << 22>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<23>", BM_rand_buf, arp_copy_to<0x10,1U << 23>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<24>", BM_rand_buf, arp_copy_to<0x10,1U << 24>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<25>", BM_rand_buf, arp_copy_to<0x10,1U << 25>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<26>", BM_rand_buf, arp_copy_to<0x10,1U << 26>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<27>", BM_rand_buf, arp_copy_to<0x10,1U << 27>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<28>", BM_rand_buf, arp_copy_to<0x10,1U << 28>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<29>", BM_rand_buf, arp_copy_to<0x10,1U << 29>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<30>", BM_rand_buf, arp_copy_to<0x10,1U << 30>, buf_size);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<31>", BM_rand_buf, arp_copy_to<0x10,1U << 31>, buf_size);
		}
	}
	else
	{
		for (size_t i = 16; i <= 256; i *= 2)
		//for (size_t i = 1; i <= 256; i *= 2)
		{
			buf_size = i;
			prefix = "rand_buf:" + std::to_string(i) + "B:";
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_1>", BM_rand_buf, arp_copy_to<0x10,1U <<  1>, buf_size)->Threads(num_threads);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_2>", BM_rand_buf, arp_copy_to<0x10,1U <<  2>, buf_size)->Threads(num_threads);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_3>", BM_rand_buf, arp_copy_to<0x10,1U <<  3>, buf_size)->Threads(num_threads);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_4>", BM_rand_buf, arp_copy_to<0x10,1U <<  4>, buf_size)->Threads(num_threads);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_5>", BM_rand_buf, arp_copy_to<0x10,1U <<  5>, buf_size)->Threads(num_threads);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_6>", BM_rand_buf, arp_copy_to<0x10,1U <<  6>, buf_size)->Threads(num_threads);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_7>", BM_rand_buf, arp_copy_to<0x10,1U <<  7>, buf_size)->Threads(num_threads);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_8>", BM_rand_buf, arp_copy_to<0x10,1U <<  8>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_9>", BM_rand_buf, arp_copy_to<0x10,1U <<  9>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<10>", BM_rand_buf, arp_copy_to<0x10,1U << 10>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<11>", BM_rand_buf, arp_copy_to<0x10,1U << 11>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<12>", BM_rand_buf, arp_copy_to<0x10,1U << 12>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<13>", BM_rand_buf, arp_copy_to<0x10,1U << 13>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<14>", BM_rand_buf, arp_copy_to<0x10,1U << 14>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<15>", BM_rand_buf, arp_copy_to<0x10,1U << 15>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<16>", BM_rand_buf, arp_copy_to<0x10,1U << 16>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<17>", BM_rand_buf, arp_copy_to<0x10,1U << 17>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<18>", BM_rand_buf, arp_copy_to<0x10,1U << 18>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<19>", BM_rand_buf, arp_copy_to<0x10,1U << 19>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<20>", BM_rand_buf, arp_copy_to<0x10,1U << 20>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<21>", BM_rand_buf, arp_copy_to<0x10,1U << 21>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<22>", BM_rand_buf, arp_copy_to<0x10,1U << 22>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<23>", BM_rand_buf, arp_copy_to<0x10,1U << 23>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<24>", BM_rand_buf, arp_copy_to<0x10,1U << 24>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<25>", BM_rand_buf, arp_copy_to<0x10,1U << 25>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<26>", BM_rand_buf, arp_copy_to<0x10,1U << 26>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<27>", BM_rand_buf, arp_copy_to<0x10,1U << 27>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<28>", BM_rand_buf, arp_copy_to<0x10,1U << 28>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<29>", BM_rand_buf, arp_copy_to<0x10,1U << 29>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<30>", BM_rand_buf, arp_copy_to<0x10,1U << 30>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<31>", BM_rand_buf, arp_copy_to<0x10,1U << 31>, buf_size)->Threads(num_threads);
		}

		//for (size_t i = 1; i <= 8; ++i)
		for (size_t i = 1; i <= 1; ++i)
		{
			buf_size = 4096 * i;
			prefix = "rand_buf:" + std::to_string(i) + "pg:";
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_1>", BM_rand_buf, arp_copy_to<0x10,1U <<  1>, buf_size)->Threads(num_threads);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_2>", BM_rand_buf, arp_copy_to<0x10,1U <<  2>, buf_size)->Threads(num_threads);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_3>", BM_rand_buf, arp_copy_to<0x10,1U <<  3>, buf_size)->Threads(num_threads);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_4>", BM_rand_buf, arp_copy_to<0x10,1U <<  4>, buf_size)->Threads(num_threads);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_5>", BM_rand_buf, arp_copy_to<0x10,1U <<  5>, buf_size)->Threads(num_threads);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_6>", BM_rand_buf, arp_copy_to<0x10,1U <<  6>, buf_size)->Threads(num_threads);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_7>", BM_rand_buf, arp_copy_to<0x10,1U <<  7>, buf_size)->Threads(num_threads);
			//benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_8>", BM_rand_buf, arp_copy_to<0x10,1U <<  8>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<_9>", BM_rand_buf, arp_copy_to<0x10,1U <<  9>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<10>", BM_rand_buf, arp_copy_to<0x10,1U << 10>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<11>", BM_rand_buf, arp_copy_to<0x10,1U << 11>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<12>", BM_rand_buf, arp_copy_to<0x10,1U << 12>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<13>", BM_rand_buf, arp_copy_to<0x10,1U << 13>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<14>", BM_rand_buf, arp_copy_to<0x10,1U << 14>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<15>", BM_rand_buf, arp_copy_to<0x10,1U << 15>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<16>", BM_rand_buf, arp_copy_to<0x10,1U << 16>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<17>", BM_rand_buf, arp_copy_to<0x10,1U << 17>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<18>", BM_rand_buf, arp_copy_to<0x10,1U << 18>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<19>", BM_rand_buf, arp_copy_to<0x10,1U << 19>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<20>", BM_rand_buf, arp_copy_to<0x10,1U << 20>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<21>", BM_rand_buf, arp_copy_to<0x10,1U << 21>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<22>", BM_rand_buf, arp_copy_to<0x10,1U << 22>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<23>", BM_rand_buf, arp_copy_to<0x10,1U << 23>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<24>", BM_rand_buf, arp_copy_to<0x10,1U << 24>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<25>", BM_rand_buf, arp_copy_to<0x10,1U << 25>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<26>", BM_rand_buf, arp_copy_to<0x10,1U << 26>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<27>", BM_rand_buf, arp_copy_to<0x10,1U << 27>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<28>", BM_rand_buf, arp_copy_to<0x10,1U << 28>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<29>", BM_rand_buf, arp_copy_to<0x10,1U << 29>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<30>", BM_rand_buf, arp_copy_to<0x10,1U << 30>, buf_size)->Threads(num_threads);
			benchmark::RegisterBenchmark(prefix + "arp_copy_to<0x10,1U<<31>", BM_rand_buf, arp_copy_to<0x10,1U << 31>, buf_size)->Threads(num_threads);
		}
	}

	benchmark::RunSpecifiedBenchmarks();
	benchmark::Shutdown();

	// }}}

	return 0;
}
