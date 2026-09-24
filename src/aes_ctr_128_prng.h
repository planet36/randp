// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// AES-CTR-128 PRNG
/**
* \file
* \author Steven Ward
*/

#pragma once

#if defined(__x86_64__) && defined(__AES__) && defined(__SSE4_1__)

#include "aes-128-utils.h"
#include "aes_ctr_128_prng-defaults.h"
#include "sha2_iv.h"
#include "wyprimes.h"

#include <err.h>
#include <immintrin.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(__cplusplus)
extern "C" {
#endif

constexpr auto AES_CTR_128_PRNG_NUM_KEYS = DEFAULT_AES_CTR_128_PRNG_NUM_KEYS;
constexpr auto AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY = DEFAULT_AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY;

/// A PRNG that uses AES instructions
struct aes_ctr_128_prng
{
    static_assert(AES_CTR_128_PRNG_NUM_KEYS >= 1);
    static_assert(AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY >= 1);
    static_assert(AES_CTR_128_PRNG_NUM_KEYS * AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY >= 3,
                  "must do at least 3 rounds of AES enc/dec");

    __m128i keys[AES_CTR_128_PRNG_NUM_KEYS]; ///< The round keys
    __m128i ctr; ///< The state/counter
};

typedef struct aes_ctr_128_prng aes_ctr_128_prng;

static_assert(sizeof(aes_ctr_128_prng) <= 256,
              "getentropy will fail if more than 256 bytes are requested");

/// Assign random bytes to the data members via \c getentropy.
/**
* \param this_ the PRNG state
*
* Each key is then adjusted, if necessary, so that its 64-bit lanes differ.
*
* \note This function terminates the calling process upon catastrophic error.
*/
static void
aes_ctr_128_prng_reseed(aes_ctr_128_prng* this_)
{
    if (getentropy(this_, sizeof(*this_)) < 0)
        err(EXIT_FAILURE, "getentropy");

    for (int i = 0; i < AES_CTR_128_PRNG_NUM_KEYS; ++i)
    {
        // The 64-bit lanes of the key must differ.
        // With a key of (K, K), if the counter
        // (A, B) gives the output (X, Y), then the counter (B, A) gives the output (Y, X).

        // most significant elem first
        const __m128i key_mask = _mm_set_epi64x((int64_t)SHA_512_H0_1, (int64_t)SHA_512_H0_0);

        const __m128i swapped = _mm_shuffle_epi32(this_->keys[i], _MM_SHUFFLE(1, 0, 3, 2));
        // all ones if the lanes are equal, all zeros otherwise
        const __m128i equal_mask = _mm_cmpeq_epi64(this_->keys[i], swapped);

        this_->keys[i] = _mm_xor_si128(this_->keys[i], _mm_and_si128(equal_mask, key_mask));
    }
}

/// Fill \a dst with \a n PRNG outputs via AES encryption
/**
* \param this_ the PRNG state
* \param dst the destination blocks
* \param n the number of blocks to fill
*
* The counter is kept in a local variable, because the stores to \a dst could alias
* \c this_->ctr.  Without it, the compiler reloads and stores the counter on every
* iteration of a loop it does not fully unroll.
*
* The keys are read through \a this_ on purpose.  A local copy of them could be spilled
* to the stack, where nothing wipes it.
*
* The counter increment \c inc used below forms a Weyl sequence.
* Criteria for its 64-bit lane values:
*   1) Must be odd
*   2) Must be unique across lanes
*
* \sa https://en.wikipedia.org/wiki/Weyl_sequence#In_computing
*/
static inline void
aes_ctr_128_prng_enc_fill(aes_ctr_128_prng* this_, __m128i* dst, const int n)
{
    const __m128i inc = wyprimes_vec128_01();

    __m128i ctr = this_->ctr;

    for (int i = 0; i < n; ++i)
    {
        dst[i] = aes_enc_128(ctr, this_->keys, AES_CTR_128_PRNG_NUM_KEYS,
                             AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY);

        ctr = _mm_add_epi64(ctr, inc);
    }

    this_->ctr = ctr;
}

/// Fill \a dst with \a n PRNG outputs via AES decryption
/**
* \param this_ the PRNG state
* \param dst the destination blocks
* \param n the number of blocks to fill
*
* Only the counter is kept in a local variable, as in \c aes_ctr_128_prng_enc_fill.
*/
static inline void
aes_ctr_128_prng_dec_fill(aes_ctr_128_prng* this_, __m128i* dst, const int n)
{
    const __m128i inc = wyprimes_vec128_01();

    __m128i ctr = this_->ctr;

    for (int i = 0; i < n; ++i)
    {
        dst[i] = aes_dec_128(ctr, this_->keys, AES_CTR_128_PRNG_NUM_KEYS,
                             AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY);

        ctr = _mm_add_epi64(ctr, inc);
    }

    this_->ctr = ctr;
}

/// Fill \a dst with \a n PRNG outputs via AES encryption and Davies-Meyer single-block-length
/// compression function
/**
* \param this_ the PRNG state
* \param dst the destination blocks
* \param n the number of blocks to fill
*
* Only the counter is kept in a local variable, as in \c aes_ctr_128_prng_enc_fill.
*/
static inline void
aes_ctr_128_prng_enc_davies_meyer_fill(aes_ctr_128_prng* this_, __m128i* dst, const int n)
{
    const __m128i inc = wyprimes_vec128_01();

    __m128i ctr = this_->ctr;

    for (int i = 0; i < n; ++i)
    {
        dst[i] = aes_enc_davies_meyer_128(ctr, this_->keys, AES_CTR_128_PRNG_NUM_KEYS,
                                          AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY);

        ctr = _mm_add_epi64(ctr, inc);
    }

    this_->ctr = ctr;
}

/// Fill \a dst with \a n PRNG outputs via AES decryption and Davies-Meyer single-block-length
/// compression function
/**
* \param this_ the PRNG state
* \param dst the destination blocks
* \param n the number of blocks to fill
*
* Only the counter is kept in a local variable, as in \c aes_ctr_128_prng_enc_fill.
*/
static inline void
aes_ctr_128_prng_dec_davies_meyer_fill(aes_ctr_128_prng* this_, __m128i* dst, const int n)
{
    const __m128i inc = wyprimes_vec128_01();

    __m128i ctr = this_->ctr;

    for (int i = 0; i < n; ++i)
    {
        dst[i] = aes_dec_davies_meyer_128(ctr, this_->keys, AES_CTR_128_PRNG_NUM_KEYS,
                                          AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY);

        ctr = _mm_add_epi64(ctr, inc);
    }

    this_->ctr = ctr;
}

#if defined(__cplusplus)
}
#endif

#endif
