// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

/// AES-CTR-128 PRNG
/**
\file
\author Steven Ward
*/

#pragma once

#include "aes128-utils.h"
#include "aes_ctr_128_prng-defaults.h"
#include "make_odd.h"

#include <err.h>
#include <immintrin.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(AESCTR128_PRNG_NUM_KEYS)
#define AESCTR128_PRNG_NUM_KEYS DEFAULT_AESCTR128_PRNG_NUM_KEYS
#endif

#if !defined(AESCTR128_PRNG_NUM_ROUNDS_PER_KEY)
#define AESCTR128_PRNG_NUM_ROUNDS_PER_KEY DEFAULT_AESCTR128_PRNG_NUM_ROUNDS_PER_KEY
#endif

/// A PRNG that uses AES instructions
struct aes_ctr_128_prng
{
    static_assert(AESCTR128_PRNG_NUM_KEYS >= 1);
    static_assert(AESCTR128_PRNG_NUM_ROUNDS_PER_KEY >= 1);
    static_assert(AESCTR128_PRNG_NUM_KEYS * AESCTR128_PRNG_NUM_ROUNDS_PER_KEY >= 2,
                  "must do at least 2 rounds of AES enc/dec");

    __m128i keys[AESCTR128_PRNG_NUM_KEYS];
    __m128i ctr; ///< The state/counter
    __m128i inc; ///< The increment (must be odd)
};

typedef struct aes_ctr_128_prng aes_ctr_128_prng;

static_assert(sizeof(aes_ctr_128_prng) <= 256,
              "getentropy will fail if more than 256 bytes are requested");

/// Assign random bytes to the data members via \c getentropy.
/**
* Every odd integer is coprime with every power of 2.
* Therefore, \c inc shall be made odd.
*/
static void
aes_ctr_128_prng_reseed(aes_ctr_128_prng* this_)
{
    if (getentropy(this_, sizeof(*this_)) < 0)
        err(EXIT_FAILURE, "getentropy");
    this_->inc = mm_make_odd_epu64(this_->inc);
}

/// Get the next PRNG output via AES encryption.
static inline __m128i
aes_ctr_128_prng_enc_next(aes_ctr_128_prng* this_)
{
    const __m128i dst = aes128_enc(this_->ctr, this_->keys,
                                   AESCTR128_PRNG_NUM_KEYS,
                                   AESCTR128_PRNG_NUM_ROUNDS_PER_KEY);
    this_->ctr = _mm_add_epi64(this_->ctr, this_->inc);
    return dst;
}

/// Get the next PRNG output via AES decryption.
static inline __m128i
aes_ctr_128_prng_dec_next(aes_ctr_128_prng* this_)
{
    const __m128i dst = aes128_dec(this_->ctr, this_->keys,
                                   AESCTR128_PRNG_NUM_KEYS,
                                   AESCTR128_PRNG_NUM_ROUNDS_PER_KEY);
    this_->ctr = _mm_add_epi64(this_->ctr, this_->inc);
    return dst;
}

/// Get the next PRNG output via AES encryption and Davies-Meyer single-block-length compression function.
static inline __m128i
aes_ctr_128_prng_enc_davies_meyer_next(aes_ctr_128_prng* this_)
{
    const __m128i dst = aes128_enc_davies_meyer(this_->ctr, this_->keys,
                                                AESCTR128_PRNG_NUM_KEYS,
                                                AESCTR128_PRNG_NUM_ROUNDS_PER_KEY);
    this_->ctr = _mm_add_epi64(this_->ctr, this_->inc);
    return dst;
}

/// Get the next PRNG output via AES decryption and Davies-Meyer single-block-length compression function.
static inline __m128i
aes_ctr_128_prng_dec_davies_meyer_next(aes_ctr_128_prng* this_)
{
    const __m128i dst = aes128_dec_davies_meyer(this_->ctr, this_->keys,
                                                AESCTR128_PRNG_NUM_KEYS,
                                                AESCTR128_PRNG_NUM_ROUNDS_PER_KEY);
    this_->ctr = _mm_add_epi64(this_->ctr, this_->inc);
    return dst;
}

#ifdef __cplusplus
}
#endif
