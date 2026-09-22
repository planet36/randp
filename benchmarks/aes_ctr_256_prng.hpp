// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// AES-CTR-256 PRNG (This C++ implementation is used only for benchmarking.)
/**
* \file
* \author Steven Ward
*
* The 256 in the name is the vector width (VAES), not the AES key size.
*
* The raison d'etre of this class is to test
* 1. different values of \c AES_CTR_256_PRNG_NUM_KEYS and \c AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY
*/

#pragma once

#include "../src/aes-256-utils.h"
#include "../src/sha2_iv.h"
#include "../src/wyprimes.h"

#include <err.h>
#include <immintrin.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

/// A PRNG that uses VAES instructions
/**
* \tparam enc if \c true, use AES encryption, otherwise AES decryption
* \tparam dm if \c true, use the Davies-Meyer single-block-length compression function (in addition to AES encryption/decryption) to get the next PRNG output
* \tparam AES_CTR_256_PRNG_NUM_KEYS the number of independent AES keys
* \tparam AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY the number of AES enc/dec rounds applied per key
*
* Each output is two 128-bit AES blocks, one in each 128-bit half of a \c __m256i.
* VAES encrypts each half independently, with the matching half of each key as its round key.
* The 256 in the name is the vector width, not the AES key size.
*/
template <bool enc,
          bool dm,
          int AES_CTR_256_PRNG_NUM_KEYS,
          int AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY>
struct aes_ctr_256_prng
{
    static_assert(AES_CTR_256_PRNG_NUM_KEYS >= 1);
    static_assert(AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY >= 1);
    static_assert(AES_CTR_256_PRNG_NUM_KEYS * AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY >= 3,
                  "must do at least 3 rounds of AES enc/dec");

private:
    __m256i keys[AES_CTR_256_PRNG_NUM_KEYS];
    __m256i ctr; ///< The state/counter

public:
    /// Construct a PRNG seeded via \c getentropy.
    /**
    * \note This function terminates the calling process upon catastrophic error.
    */
    aes_ctr_256_prng()
    {
        static_assert(sizeof(*this) <= 256,
                      "getentropy will fail if more than 256 bytes are requested");
        reseed();
    }

    /// Assign random bytes to the data members via \c getentropy.
    /**
    * Each key is then adjusted, if necessary, so that the two 64-bit lanes of each 128-bit
    * half differ.
    *
    * \note This function terminates the calling process upon catastrophic error.
    */
    void reseed() noexcept
    {
        if (getentropy(this, sizeof(*this)) < 0)
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
            const __m256i key_mask = _mm256_set_epi64x((int64_t)SHA_512_H0_3,
                    (int64_t)SHA_512_H0_2, (int64_t)SHA_512_H0_1, (int64_t)SHA_512_H0_0);

            const __m256i swapped = _mm256_shuffle_epi32(this->keys[i], _MM_SHUFFLE(1, 0, 3, 2));
            // all ones if the lanes are equal, all zeros otherwise
            const __m256i equal_mask = _mm256_cmpeq_epi64(this->keys[i], swapped);

            this->keys[i] = _mm256_xor_si256(this->keys[i],
                                             _mm256_and_si256(equal_mask, key_mask));
        }
#else
#error "Architecture not supported"
#endif
    }

    /// Get the next PRNG output via AES encryption or decryption.
    /**
    * \return the next PRNG output
    *
    * The counter increment \c inc used below forms a Weyl sequence.
    * Criteria for its 64-bit lane values:
    *   1) Must be odd
    *   2) Must be unique across lanes
    *
    * \sa https://en.wikipedia.org/wiki/Weyl_sequence#In_computing
    */
    [[nodiscard]] __m256i next() noexcept
    {
        const __m256i inc = wyprimes_vec256();

        __m256i dst;

        if constexpr (enc)
        {
            if constexpr (dm)
                dst = aes_enc_davies_meyer_256(this->ctr, this->keys, AES_CTR_256_PRNG_NUM_KEYS,
                                              AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY);
            else
                dst = aes_enc_256(this->ctr, this->keys, AES_CTR_256_PRNG_NUM_KEYS,
                                 AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY);
        }
        else
        {
            if constexpr (dm)
                dst = aes_dec_davies_meyer_256(this->ctr, this->keys, AES_CTR_256_PRNG_NUM_KEYS,
                                              AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY);
            else
                dst = aes_dec_256(this->ctr, this->keys, AES_CTR_256_PRNG_NUM_KEYS,
                                 AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY);
        }

        this->ctr = _mm256_add_epi64(this->ctr, inc);

        return dst;
    }
};
