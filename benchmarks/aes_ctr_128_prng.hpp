// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// AES-CTR-128 PRNG (This C++ implementation is used only for benchmarking.)
/**
\file
\author Steven Ward

The raison d'etre of this class is to test
1. different values of \c AESCTR128_PRNG_NUM_KEYS and \c AESCTR128_PRNG_NUM_ROUNDS_PER_KEY
*/

#pragma once

#include "../src/aes128-utils.h"
#include "../src/make_odd.h"

#include <err.h>
#include <immintrin.h>
#include <stdlib.h>
#include <unistd.h>

/// A PRNG that uses AES instructions
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
    __m128i inc; ///< The increment (must be odd)

public:
    aes_ctr_128_prng()
    {
        static_assert(sizeof(*this) <= 256,
                      "getentropy will fail if more than 256 bytes are requested");
        reseed();
    }

    /// Assign random bytes to the data members via \c getentropy.
    /**
    * Every odd integer is coprime with every power of 2.
    * Therefore, \c inc shall be made odd.
    */
    void reseed()
    {
        if (getentropy(this, sizeof(*this)) < 0)
            err(EXIT_FAILURE, "getentropy");
        this->inc = mm_make_odd_epu64(this->inc);
    }

    /// Get the next PRNG output via AES encryption or decryption.
    __m128i next()
    {
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
        this->ctr = _mm_add_epi64(this->ctr, this->inc);
        return dst;
    }
};
