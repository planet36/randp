// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// AES-CTR-256 PRNG
/**
* \file
* \author Steven Ward
*
* The 256 in the name is the vector width (VAES), not the AES key size.
*/

#pragma once

#if defined(__x86_64__) && defined(__VAES__) && defined(__AVX2__)

#include "aes-256-utils.h"
#include "aes_ctr_256_prng-defaults.h"

#include <err.h>
#include <immintrin.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(__cplusplus)
extern "C" {
#endif

constexpr auto AES_CTR_256_PRNG_NUM_KEYS = DEFAULT_AES_CTR_256_PRNG_NUM_KEYS;
constexpr auto AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY =
    DEFAULT_AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY;

/// A PRNG that uses VAES instructions
/**
* Each output is two 128-bit AES blocks, one in each 128-bit half of a \c __m256i.
* VAES encrypts each half independently, with the matching half of each key as its round key.
* The 256 in the name is the vector width, not the AES key size.
*/
struct aes_ctr_256_prng
{
    static_assert(AES_CTR_256_PRNG_NUM_KEYS >= 1);
    static_assert(AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY >= 1);
    static_assert(AES_CTR_256_PRNG_NUM_KEYS * AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY >= 3,
                  "must do at least 3 rounds of AES enc/dec");

    __m256i keys[AES_CTR_256_PRNG_NUM_KEYS]; ///< The round keys
    __m256i ctr;                             ///< The state/counter
};

typedef struct aes_ctr_256_prng aes_ctr_256_prng;

static_assert(sizeof(aes_ctr_256_prng) <= 256,
              "getentropy will fail if more than 256 bytes are requested");

/// Assign random bytes to the data members via \c getentropy.
/**
* \param this_ the PRNG state
*
* Each key is then adjusted, if necessary, so that the two 64-bit lanes of each 128-bit half
* differ.
*
* \note This function terminates the calling process upon catastrophic error.
*/
static void
aes_ctr_256_prng_reseed(aes_ctr_256_prng* this_)
{
    if (getentropy(this_, sizeof(*this_)) < 0)
        err(EXIT_FAILURE, "getentropy");

    for (int i = 0; i < AES_CTR_256_PRNG_NUM_KEYS; ++i)
    {
        this_->keys[i] = rectify_key_256(this_->keys[i]);
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
aes_ctr_256_prng_enc_fill(aes_ctr_256_prng* this_, __m256i* dst, const int n)
{
    const __m256i inc = wyprimes_vec256();

    __m256i ctr = this_->ctr;

    for (int i = 0; i < n; ++i)
    {
        dst[i] = aes_enc_256(ctr, this_->keys, AES_CTR_256_PRNG_NUM_KEYS,
                             AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY);

        ctr = _mm256_add_epi64(ctr, inc);
    }

    this_->ctr = ctr;
}

/// Fill \a dst with \a n PRNG outputs via AES decryption
/**
* \param this_ the PRNG state
* \param dst the destination blocks
* \param n the number of blocks to fill
*
* Only the counter is kept in a local variable, as in \c aes_ctr_256_prng_enc_fill.
*/
static inline void
aes_ctr_256_prng_dec_fill(aes_ctr_256_prng* this_, __m256i* dst, const int n)
{
    const __m256i inc = wyprimes_vec256();

    __m256i ctr = this_->ctr;

    for (int i = 0; i < n; ++i)
    {
        dst[i] = aes_dec_256(ctr, this_->keys, AES_CTR_256_PRNG_NUM_KEYS,
                             AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY);

        ctr = _mm256_add_epi64(ctr, inc);
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
* Only the counter is kept in a local variable, as in \c aes_ctr_256_prng_enc_fill.
*/
static inline void
aes_ctr_256_prng_enc_davies_meyer_fill(aes_ctr_256_prng* this_, __m256i* dst, const int n)
{
    const __m256i inc = wyprimes_vec256();

    __m256i ctr = this_->ctr;

    for (int i = 0; i < n; ++i)
    {
        dst[i] = aes_enc_davies_meyer_256(ctr, this_->keys, AES_CTR_256_PRNG_NUM_KEYS,
                                          AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY);

        ctr = _mm256_add_epi64(ctr, inc);
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
* Only the counter is kept in a local variable, as in \c aes_ctr_256_prng_enc_fill.
*/
static inline void
aes_ctr_256_prng_dec_davies_meyer_fill(aes_ctr_256_prng* this_, __m256i* dst, const int n)
{
    const __m256i inc = wyprimes_vec256();

    __m256i ctr = this_->ctr;

    for (int i = 0; i < n; ++i)
    {
        dst[i] = aes_dec_davies_meyer_256(ctr, this_->keys, AES_CTR_256_PRNG_NUM_KEYS,
                                          AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY);

        ctr = _mm256_add_epi64(ctr, inc);
    }

    this_->ctr = ctr;
}

#if defined(__cplusplus)
}
#endif

#endif
