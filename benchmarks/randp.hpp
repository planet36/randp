// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

/// AES-CTR Random Pool (randp) (This C++ implementation is used only for benchmarking.)
/**
\file
\author Steven Ward
Inspired by \c arc4random.
\sa https://github.com/openbsd/src/blob/master/lib/libc/crypt/arc4random.c
\sa https://man7.org/linux/man-pages/man3/arc4random.3.html

The raisons d'etre of this class are to test
1. different values of \c RANDP_NUM_BLOCKS and \c RANDP_RESEED_COUNTDOWN_MIN
2. using a mutex for the static randp data instead of a \c thread_local instance
*/

#pragma once

#include "../src/aes128_prng-defaults.h"
#include "../src/allocate.h"
#include "../src/randp-defaults.h"
#include "aes128_prng.hpp"

#include <bit>
#include <immintrin.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/user.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

/// A pool of random bytes
template <
    unsigned int RANDP_NUM_BLOCKS = DEFAULT_RANDP_NUM_BLOCKS,
    unsigned int RANDP_RESEED_COUNTDOWN_MIN = DEFAULT_RANDP_RESEED_COUNTDOWN_MIN,
    // {{{ PRNG params
    bool enc = DEFAULT_RANDP_PRNG_USE_ENC,
    bool dm = DEFAULT_RANDP_PRNG_USE_DAVIES_MEYER,
    size_t Nk = DEFAULT_AES128_PRNG_NUM_KEYS,
    size_t Nr = DEFAULT_AES128_PRNG_NUM_ROUNDS_PER_KEY
    // }}}
>
struct randp
{
	static_assert(RANDP_NUM_BLOCKS >= 1);
	static_assert(std::has_single_bit(RANDP_RESEED_COUNTDOWN_MIN));
	static constexpr size_t RANDP_NUM_BYTES =
	    RANDP_NUM_BLOCKS * sizeof(__m128i);
	aes128_prng<enc, dm, Nk, Nr> prng;
	size_t reseed_countdown;     // The PRNG is reseeded when this is 0.
	size_t rand_bytes_remaining; // The pool is regenerated when this is 0.
	uint8_t pool[RANDP_NUM_BYTES];

	void regen()
	{
		if (this->reseed_countdown == 0)
		{
			prng.reseed();
			this->reseed_countdown = RANDP_RESEED_COUNTDOWN_MIN;
			// eliminate variations
#if 0
			this->reseed_countdown +=
			    (__builtin_ia32_rdtsc() % RANDP_RESEED_COUNTDOWN_MIN) / 2;
#endif
		}

		__m128i* blocks = (__m128i*)(&this->pool[0]);

		for (size_t i = 0; i < RANDP_NUM_BLOCKS; ++i)
		{
			blocks[i] = prng.next();
		}

		this->rand_bytes_remaining = RANDP_NUM_BYTES;
		--this->reseed_countdown;
	}
};

template <
    unsigned int RANDP_NUM_BLOCKS = DEFAULT_RANDP_NUM_BLOCKS,
    unsigned int RANDP_RESEED_COUNTDOWN_MIN = DEFAULT_RANDP_RESEED_COUNTDOWN_MIN,
    // {{{ PRNG params
    bool enc = DEFAULT_RANDP_PRNG_USE_ENC,
    bool dm = DEFAULT_RANDP_PRNG_USE_DAVIES_MEYER,
    size_t Nk = DEFAULT_AES128_PRNG_NUM_KEYS,
    size_t Nr = DEFAULT_AES128_PRNG_NUM_ROUNDS_PER_KEY
    // }}}
>
void
randp_bytes(void* buf, size_t n)
{
	using randp_t =
	    randp<RANDP_NUM_BLOCKS, RANDP_RESEED_COUNTDOWN_MIN, enc, dm, Nk, Nr>;

	static thread_local randp_t* this_ = nullptr;

	static_assert(alignof(randp_t) == sizeof(__m128i),
	              "randp must have alignment of __m128i");

	static_assert(offsetof(randp_t, pool) % sizeof(__m128i) == 0,
	              "randp pool must start on 16-byte boundary");

	static_assert(sizeof(randp_t) <= PAGE_SIZE, "randp must fit in one page");

	if (this_ == nullptr)
#ifdef __cplusplus
		this_ = (decltype(this_))allocate(sizeof(*this_));
#else
		this_ = (typeof(this_))allocate(sizeof(*this_));
#endif

	uint8_t* dst = (uint8_t*)buf;

	while (n > 0)
	{
		if (this_->rand_bytes_remaining == 0)
			this_->regen();

		uint8_t* src =
		    &this_->pool[this_->RANDP_NUM_BYTES - this_->rand_bytes_remaining];

		const size_t m = MIN(n, this_->rand_bytes_remaining);

		(void)memcpy(dst, src, m);
#if defined(memset_explicit)
		(void)memset_explicit(src, 0, m);
#elif defined(explicit_bzero)
		explicit_bzero(src, m);
#else
		(void)memset(src, 0, m);
#endif

		dst += m;
		this_->rand_bytes_remaining -= m;
		n -= m;
	}
}

// {{{ This uses a global mutex instead of thread_local keyword for the randp*.

#include <pthread.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
static pthread_mutex_t randp_mtx = PTHREAD_MUTEX_INITIALIZER;
#pragma GCC diagnostic pop

template <
    unsigned int RANDP_NUM_BLOCKS = DEFAULT_RANDP_NUM_BLOCKS,
    unsigned int RANDP_RESEED_COUNTDOWN_MIN = DEFAULT_RANDP_RESEED_COUNTDOWN_MIN,
    // {{{ PRNG params
    bool enc = DEFAULT_RANDP_PRNG_USE_ENC,
    bool dm = DEFAULT_RANDP_PRNG_USE_DAVIES_MEYER,
    size_t Nk = DEFAULT_AES128_PRNG_NUM_KEYS,
    size_t Nr = DEFAULT_AES128_PRNG_NUM_ROUNDS_PER_KEY
    // }}}
>
void
randp_bytes_MUTEX(void* buf, size_t n)
{
	using randp_t =
	    randp<RANDP_NUM_BLOCKS, RANDP_RESEED_COUNTDOWN_MIN, enc, dm, Nk, Nr>;

	// Intentionally not thread_local
	static randp_t* this_ = nullptr;

	static_assert(alignof(randp_t) == sizeof(__m128i),
	              "randp must have alignment of __m128i");

	static_assert(offsetof(randp_t, pool) % sizeof(__m128i) == 0,
	              "randp pool must start on 16-byte boundary");

	static_assert(sizeof(randp_t) <= PAGE_SIZE, "randp must fit in one page");

	(void)pthread_mutex_lock(&randp_mtx);

	if (this_ == nullptr)
#ifdef __cplusplus
		this_ = (decltype(this_))allocate(sizeof(*this_));
#else
		this_ = (typeof(this_))allocate(sizeof(*this_));
#endif

	uint8_t* dst = (uint8_t*)buf;

	while (n > 0)
	{
		if (this_->rand_bytes_remaining == 0)
			this_->regen();

		uint8_t* src =
		    &this_->pool[this_->RANDP_NUM_BYTES - this_->rand_bytes_remaining];

		const size_t m = MIN(n, this_->rand_bytes_remaining);

		(void)memcpy(dst, src, m);
#if defined(memset_explicit)
		(void)memset_explicit(src, 0, m);
#elif defined(explicit_bzero)
		explicit_bzero(src, m);
#else
		(void)memset(src, 0, m);
#endif

		dst += m;
		this_->rand_bytes_remaining -= m;
		n -= m;
	}

	(void)pthread_mutex_unlock(&randp_mtx);
}

// }}}
