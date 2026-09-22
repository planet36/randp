// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// AES-CTR-256 PRNG
/**
* \file
* \author Steven Ward
*/

#pragma once

#include "aes-256-utils.h"
#include "aes_ctr_256_prng-defaults.h"
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

#if !defined(AES_CTR_256_PRNG_NUM_KEYS)
#define AES_CTR_256_PRNG_NUM_KEYS DEFAULT_AES_CTR_256_PRNG_NUM_KEYS
#endif

#if !defined(AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY)
#define AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY DEFAULT_AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY
#endif

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
    __m256i ctr; ///< The state/counter
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

#if defined(__x86_64__) && defined(__AVX2__)
    for (int i = 0; i < AES_CTR_256_PRNG_NUM_KEYS; ++i)
    {
        // The two 64-bit lanes of each 128-bit half of the key must differ.
        // With a key half of (K, K), if the counter half (A, B) gives the output (X, Y),
        // then the counter half (B, A) gives the output (Y, X).
        // The shuffle and compare below work within each 128-bit half, so they check each
        // half on its own.

        // most significant elem first
        const __m256i key_mask = _mm256_set_epi64x((int64_t)SHA_512_H0_3, (int64_t)SHA_512_H0_2,
                (int64_t)SHA_512_H0_1, (int64_t)SHA_512_H0_0);

        const __m256i swapped = _mm256_shuffle_epi32(this_->keys[i], _MM_SHUFFLE(1, 0, 3, 2));
        // all ones if the lanes are equal, all zeros otherwise
        const __m256i equal_mask = _mm256_cmpeq_epi64(this_->keys[i], swapped);

        this_->keys[i] = _mm256_xor_si256(this_->keys[i],
                                          _mm256_and_si256(equal_mask, key_mask));
    }
#else
#error "Architecture not supported"
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
[[nodiscard]] static inline __m256i
aes_ctr_256_prng_enc_next(aes_ctr_256_prng* this_)
{
    const __m256i inc = wyprimes_vec256();

    const __m256i dst = aes_enc_256(this_->ctr, this_->keys, AES_CTR_256_PRNG_NUM_KEYS,
                                   AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY);

    this_->ctr = _mm256_add_epi64(this_->ctr, inc);

    return dst;
}

/// Get the next PRNG output via AES decryption.
/**
* \param this_ the PRNG state
* \return the next PRNG output
*/
[[nodiscard]] static inline __m256i
aes_ctr_256_prng_dec_next(aes_ctr_256_prng* this_)
{
    const __m256i inc = wyprimes_vec256();

    const __m256i dst = aes_dec_256(this_->ctr, this_->keys, AES_CTR_256_PRNG_NUM_KEYS,
                                   AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY);

    this_->ctr = _mm256_add_epi64(this_->ctr, inc);

    return dst;
}

/// Get the next PRNG output via AES encryption and Davies-Meyer single-block-length
/// compression function.
/**
* \param this_ the PRNG state
* \return the next PRNG output
*/
[[nodiscard]] static inline __m256i
aes_ctr_256_prng_enc_davies_meyer_next(aes_ctr_256_prng* this_)
{
    const __m256i inc = wyprimes_vec256();

    const __m256i dst = aes_enc_davies_meyer_256(this_->ctr, this_->keys,
            AES_CTR_256_PRNG_NUM_KEYS, AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY);

    this_->ctr = _mm256_add_epi64(this_->ctr, inc);

    return dst;
}

/// Get the next PRNG output via AES decryption and Davies-Meyer single-block-length
/// compression function.
/**
* \param this_ the PRNG state
* \return the next PRNG output
*/
[[nodiscard]] static inline __m256i
aes_ctr_256_prng_dec_davies_meyer_next(aes_ctr_256_prng* this_)
{
    const __m256i inc = wyprimes_vec256();

    const __m256i dst = aes_dec_davies_meyer_256(this_->ctr, this_->keys,
            AES_CTR_256_PRNG_NUM_KEYS, AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY);

    this_->ctr = _mm256_add_epi64(this_->ctr, inc);

    return dst;
}

#if defined(__cplusplus)
}
#endif
