// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// AES-CTR-128 PRNG
/**
* \file
* \author Steven Ward
*/

#pragma once

#include "aes-128-utils.h"
#include "aes_ctr_128_prng-defaults.h"
#include "sha2_iv.h"
#include "wyprimes.h"

#include <err.h>
#include <immintrin.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(AES_CTR_128_PRNG_NUM_KEYS)
#define AES_CTR_128_PRNG_NUM_KEYS DEFAULT_AES_CTR_128_PRNG_NUM_KEYS
#endif

#if !defined(AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY)
#define AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY DEFAULT_AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY
#endif

/// A PRNG that uses AES instructions
struct aes_ctr_128_prng
{
    static_assert(AES_CTR_128_PRNG_NUM_KEYS >= 1);
    static_assert(AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY >= 1);
    static_assert(AES_CTR_128_PRNG_NUM_KEYS * AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY >= 3,
                  "must do at least 3 rounds of AES enc/dec");

    __m128i keys[AES_CTR_128_PRNG_NUM_KEYS];
    __m128i ctr; ///< The state/counter
};

typedef struct aes_ctr_128_prng aes_ctr_128_prng;

static_assert(sizeof(aes_ctr_128_prng) <= 256,
              "getentropy will fail if more than 256 bytes are requested");

/// Assign random bytes to the data members via \c getentropy.
/**
* Each key is then adjusted, if necessary, so that its 64-bit lanes differ.
*
* \note This function terminates the calling process upon catastrophic error.
*/
static void
aes_ctr_128_prng_reseed(aes_ctr_128_prng* this_)
{
    if (getentropy(this_, sizeof(*this_)) < 0)
        err(EXIT_FAILURE, "getentropy");

#if defined(__x86_64__) && defined(__SSE4_1__)
    for (int i = 0; i < AES_CTR_128_PRNG_NUM_KEYS; ++i)
    {
        // The 64-bit lanes of the key must differ.
        // With a key of (K, K), if the counter
        // (A, B) gives the output (X, Y), then the counter (B, A) gives the output (Y, X).

        // most significant elem first
        const __m128i key_mask = _mm_set_epi64x(SHA_512_H0_1, SHA_512_H0_0); // NOLINT(cppcoreguidelines-narrowing-conversions)

        const __m128i swapped = _mm_shuffle_epi32(this_->keys[i], _MM_SHUFFLE(1, 0, 3, 2));
        // all ones if the lanes are equal, all zeros otherwise
        const __m128i equal_mask = _mm_cmpeq_epi64(this_->keys[i], swapped);

        this_->keys[i] = _mm_xor_si128(this_->keys[i], _mm_and_si128(equal_mask, key_mask));
    }
#endif
}

/// Get the next PRNG output via AES encryption.
/**
* \param this_ the PRNG state
* \return the next PRNG output
*
* The counter increment \c inc used below forms a Weyl sequence.
* Criteria for its 64-bit lane values:
*   1) Must be odd
*   2) Must be unique across lanes
*
* \sa https://en.wikipedia.org/wiki/Weyl_sequence#In_computing
*/
[[nodiscard]] static inline __m128i
aes_ctr_128_prng_enc_next(aes_ctr_128_prng* this_)
{
    const __m128i inc = wyprimes_vec128_01();

    const __m128i dst = aes_enc_128(this_->ctr, this_->keys, AES_CTR_128_PRNG_NUM_KEYS,
                                   AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY);

    this_->ctr = _mm_add_epi64(this_->ctr, inc);

    return dst;
}

/// Get the next PRNG output via AES decryption.
/**
* \param this_ the PRNG state
* \return the next PRNG output
*/
[[nodiscard]] static inline __m128i
aes_ctr_128_prng_dec_next(aes_ctr_128_prng* this_)
{
    const __m128i inc = wyprimes_vec128_01();

    const __m128i dst = aes_dec_128(this_->ctr, this_->keys, AES_CTR_128_PRNG_NUM_KEYS,
                                   AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY);

    this_->ctr = _mm_add_epi64(this_->ctr, inc);

    return dst;
}

/// Get the next PRNG output via AES encryption and Davies-Meyer single-block-length compression function.
/**
* \param this_ the PRNG state
* \return the next PRNG output
*/
[[nodiscard]] static inline __m128i
aes_ctr_128_prng_enc_davies_meyer_next(aes_ctr_128_prng* this_)
{
    const __m128i inc = wyprimes_vec128_01();

    const __m128i dst = aes_enc_davies_meyer_128(this_->ctr, this_->keys,
            AES_CTR_128_PRNG_NUM_KEYS, AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY);

    this_->ctr = _mm_add_epi64(this_->ctr, inc);

    return dst;
}

/// Get the next PRNG output via AES decryption and Davies-Meyer single-block-length compression function.
/**
* \param this_ the PRNG state
* \return the next PRNG output
*/
[[nodiscard]] static inline __m128i
aes_ctr_128_prng_dec_davies_meyer_next(aes_ctr_128_prng* this_)
{
    const __m128i inc = wyprimes_vec128_01();

    const __m128i dst = aes_dec_davies_meyer_128(this_->ctr, this_->keys,
            AES_CTR_128_PRNG_NUM_KEYS, AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY);

    this_->ctr = _mm_add_epi64(this_->ctr, inc);

    return dst;
}

#if defined(__cplusplus)
}
#endif
