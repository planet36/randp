// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// AES-CTR-128 PRNG (This C++ implementation is used only for benchmarking.)
/**
* \file
* \author Steven Ward
*
* The raison d'etre of this class is to test
* 1. different values of \c Nk and \c Nr
*/

#pragma once

#include "../src/aes-128-utils.h"
#include "../src/sha2_iv.h"
#include "../src/wyprimes.h"

#include <err.h>
#include <immintrin.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

[[nodiscard]] static inline auto
add_epu64(__m128i a, __m128i b)
{
    return _mm_add_epi64(a, b);
}

#if defined(__x86_64__) && defined(__SSE4_1__)

/// Adjust \a key so that its 64-bit lanes differ
/**
* The 64-bit lanes of the key must differ.
* With a key of (K, K), if the counter
* (A, B) gives the output (X, Y), then the counter (B, A) gives the output (Y, X).
*/
[[nodiscard]] static inline auto
rectify_key(__m128i key)
{
    // most significant elem first
    const __m128i key_mask = _mm_set_epi64x((int64_t)SHA_512_H0_1, (int64_t)SHA_512_H0_0);

    const __m128i swapped = _mm_shuffle_epi32(key, _MM_SHUFFLE(1, 0, 3, 2));

    // all ones if the lanes are equal, all zeros otherwise
    const __m128i equal_mask = _mm_cmpeq_epi64(key, swapped);

    key = _mm_xor_si128(key, _mm_and_si128(equal_mask, key_mask));

    return key;
}

#else
#error "Architecture not supported"
#endif

/// A PRNG that uses AES instructions
/**
* \tparam enc if \c true, use AES encryption, otherwise AES decryption
* \tparam dm if \c true, use the Davies-Meyer single-block-length compression function (in addition to AES encryption/decryption) to get the next PRNG output
* \tparam Nk the number of independent AES keys
* \tparam Nr the number of AES enc/dec rounds applied per key
*/
template <bool enc, bool dm, int Nk, int Nr>
struct aes_ctr_128_prng
{
    static_assert(Nk >= 1);
    static_assert(Nr >= 1);
    static_assert(Nk * Nr >= 3, "must do at least 3 rounds of AES enc/dec");

    using block_t = __m128i;

private:
    block_t keys[Nk]; ///< The round keys
    block_t ctr; ///< The state/counter

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
    * Each key is then adjusted, if necessary.
    *
    * \note This function terminates the calling process upon catastrophic error.
    */
    void reseed() noexcept
    {
        if (getentropy(this, sizeof(*this)) < 0)
            err(EXIT_FAILURE, "getentropy");

        for (int i = 0; i < Nk; ++i)
        {
            this->keys[i] = rectify_key(this->keys[i]);
        }
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
    [[nodiscard]] block_t next() noexcept
    {
        const block_t inc = wyprimes_vec128_01();

        block_t dst;

        if constexpr (enc)
        {
            if constexpr (dm)
                dst = aes_enc_davies_meyer(this->ctr, this->keys, Nk, Nr);
            else
                dst = aes_enc(this->ctr, this->keys, Nk, Nr);
        }
        else
        {
            if constexpr (dm)
                dst = aes_dec_davies_meyer(this->ctr, this->keys, Nk, Nr);
            else
                dst = aes_dec(this->ctr, this->keys, Nk, Nr);
        }

        this->ctr = add_epu64(this->ctr, inc);

        return dst;
    }
};
