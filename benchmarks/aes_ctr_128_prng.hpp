// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// AES-CTR-128 PRNG (This C++ implementation is used only for benchmarking.)
/**
* \file
* \author Steven Ward
*
* The raison d'etre of this class is to test
* 1. different values of \c AESCTR128_PRNG_NUM_KEYS and \c AESCTR128_PRNG_NUM_ROUNDS_PER_KEY
*/

#pragma once

#include "../src/aes128-utils.h"
#include "../src/sha2_iv.h"

#include <err.h>
#include <immintrin.h>
#include <stdlib.h>
#include <unistd.h>

/// A PRNG that uses AES instructions
/**
* \tparam enc if \c true, use AES encryption, otherwise AES decryption
* \tparam dm if \c true, use the Davies-Meyer single-block-length compression function (in addition to AES encryption/decryption) to get the next PRNG output
* \tparam AESCTR128_PRNG_NUM_KEYS the number of independent AES keys
* \tparam AESCTR128_PRNG_NUM_ROUNDS_PER_KEY the number of AES enc/dec rounds applied per key
*/
template <bool enc,
          bool dm,
          size_t AESCTR128_PRNG_NUM_KEYS,
          size_t AESCTR128_PRNG_NUM_ROUNDS_PER_KEY>
struct aes_ctr_128_prng
{
    static_assert(AESCTR128_PRNG_NUM_KEYS >= 1);
    static_assert(AESCTR128_PRNG_NUM_ROUNDS_PER_KEY >= 1);
    static_assert(AESCTR128_PRNG_NUM_KEYS * AESCTR128_PRNG_NUM_ROUNDS_PER_KEY >= 3,
                  "must do at least 3 rounds of AES enc/dec");

private:
    __m128i keys[AESCTR128_PRNG_NUM_KEYS];
    __m128i ctr; ///< The state/counter

public:
    /// Construct a PRNG seeded via \c getentropy.
    /**
    * \note This function terminates the calling process upon catastrophic error.
    */
    aes_ctr_128_prng()
    {
        static_assert(sizeof(*this) <= 256,
                      "getentropy will fail if more than 256 bytes are requested");
        reseed();
    }

    /// Assign random bytes to the data members via \c getentropy.
    /**
    * \note This function terminates the calling process upon catastrophic error.
    */
    void reseed() noexcept
    {
        if (getentropy(this, sizeof(*this)) < 0)
            err(EXIT_FAILURE, "getentropy");
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
    __m128i next() noexcept
    {
        // most significant elem first
        const __m128i inc = _mm_set_epi64x(SHA_512_H0_1 | 1U, SHA_512_H0_0 | 1U); // NOLINT(cppcoreguidelines-narrowing-conversions)

        __m128i dst;

        if constexpr (enc)
        {
            if constexpr (dm)
                dst = aes128_enc_davies_meyer(this->ctr, this->keys, AESCTR128_PRNG_NUM_KEYS,
                                              AESCTR128_PRNG_NUM_ROUNDS_PER_KEY);
            else
                dst = aes128_enc(this->ctr, this->keys, AESCTR128_PRNG_NUM_KEYS,
                                 AESCTR128_PRNG_NUM_ROUNDS_PER_KEY);
        }
        else
        {
            if constexpr (dm)
                dst = aes128_dec_davies_meyer(this->ctr, this->keys, AESCTR128_PRNG_NUM_KEYS,
                                              AESCTR128_PRNG_NUM_ROUNDS_PER_KEY);
            else
                dst = aes128_dec(this->ctr, this->keys, AESCTR128_PRNG_NUM_KEYS,
                                 AESCTR128_PRNG_NUM_ROUNDS_PER_KEY);
        }

        this->ctr = _mm_add_epi64(this->ctr, inc);

        return dst;
    }
};
