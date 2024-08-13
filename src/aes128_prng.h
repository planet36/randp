// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

/// AES-128 PRNG
/**
\file
\author Steven Ward
*/

#pragma once

#include "aes128-utils.h"

#include <err.h>
#include <immintrin.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(AES128_PRNG_NUM_KEYS)
#define AES128_PRNG_NUM_KEYS 2
#endif

#if !defined(AES128_PRNG_NUM_ROUNDS_PER_KEY)
#define AES128_PRNG_NUM_ROUNDS_PER_KEY 1
#endif

static_assert(AES128_PRNG_NUM_KEYS >= 1);
static_assert(AES128_PRNG_NUM_ROUNDS_PER_KEY >= 1);
static_assert(AES128_PRNG_NUM_KEYS * AES128_PRNG_NUM_ROUNDS_PER_KEY >= 2,
              "must do at least 2 rounds of AES enc/dec");

/// A PRNG that uses AES instructions
struct aes128_prng
{
	__m128i x;
	__m128i c; // must be odd
	__m128i keys[AES128_PRNG_NUM_KEYS];
};

typedef struct aes128_prng aes128_prng;

static_assert(sizeof(aes128_prng) <= 256,
              "getentropy will fail if more than 256 bytes are requested");

/// Assign random bytes to the data members via \c getentropy.
/**
* Every odd integer is coprime with every power of 2.
* Therefore, \c c shall be made odd.
*/
static void
aes128_prng_reseed(aes128_prng* this_)
{
	if (getentropy(this_, sizeof(*this_)) < 0)
		err(EXIT_FAILURE, "getentropy");
	this_->c = mm_make_odd_epu64(this_->c);
}

/// Get the next PRNG output via AES encryption.
static __m128i
aes128_prng_enc_next(aes128_prng* this_)
{
	const __m128i dst = aes128_enc(this_->x, this_->keys, AES128_PRNG_NUM_KEYS,
	                               AES128_PRNG_NUM_ROUNDS_PER_KEY);
	this_->x = _mm_add_epi64(this_->x, this_->c);
	return dst;
}

#ifdef __cplusplus
}
#endif