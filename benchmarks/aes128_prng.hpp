// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

/// AES-128 PRNG (This C++ implementation is used only for benchmarking.)
/**
\file
\author Steven Ward

The raison d'etre of this class is to test
1. different values of \c AES128_PRNG_NUM_KEYS and \c AES128_PRNG_NUM_ROUNDS_PER_KEY
*/

#pragma once

#include "../src/aes128-utils.h"

#include <err.h>
#include <immintrin.h>
#include <stdlib.h>
#include <unistd.h>

/// A PRNG that uses AES instructions
template <bool enc,
          size_t AES128_PRNG_NUM_KEYS,
          size_t AES128_PRNG_NUM_ROUNDS_PER_KEY>
struct aes128_prng
{
	static_assert(AES128_PRNG_NUM_KEYS >= 1);
	static_assert(AES128_PRNG_NUM_ROUNDS_PER_KEY >= 1);
	static_assert(AES128_PRNG_NUM_KEYS * AES128_PRNG_NUM_ROUNDS_PER_KEY >= 2,
	              "must do at least 2 rounds of AES enc/dec");

private:
	__m128i x;
	__m128i c; // must be odd
	__m128i keys[AES128_PRNG_NUM_KEYS];

public:
	aes128_prng()
	{
		static_assert(sizeof(*this) <= 256,
		    "getentropy will fail if more than 256 bytes are requested");
		reseed();
	}

	/// Assign random bytes to the data members via \c getentropy.
	/**
	* Every odd integer is coprime with every power of 2.
	* Therefore, \c c shall be made odd.
	*/
	void reseed()
	{
		if (getentropy(this, sizeof(*this)) < 0)
			err(EXIT_FAILURE, "getentropy");
		this->c = mm_make_odd_epu64(this->c);
	}

	/// Get the next PRNG output via AES encryption.
	__m128i next()
	{
		__m128i dst;
		if constexpr (enc)
			dst = aes128_enc(this->x, this->keys, AES128_PRNG_NUM_KEYS,
			                 AES128_PRNG_NUM_ROUNDS_PER_KEY);
		else
			dst = aes128_dec(this->x, this->keys, AES128_PRNG_NUM_KEYS,
			                 AES128_PRNG_NUM_ROUNDS_PER_KEY);
		this->x = _mm_add_epi64(this->x, this->c);
		return dst;
	}
};