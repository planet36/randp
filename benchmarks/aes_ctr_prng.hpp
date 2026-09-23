// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// AES-CTR PRNG class (This C++ implementation is used only for benchmarking.)
/**
* \file
* \author Steven Ward
*
* The raison d'etre of this class is to test
* 1. different values of \c Nk and \c Nr
*/

#pragma once

#if !(defined(__x86_64__) && \
      defined(__AES__) && defined(__SSE4_1__) && \
      defined(__VAES__) && defined(__AVX2__))
#error "Architecture not supported"
#endif

#include "../src/aes-128-utils.h"
#include "../src/aes-256-utils.h"
#include "../src/aes_ctr_128_prng-defaults.h"
#include "../src/aes_ctr_256_prng-defaults.h"
#include "../src/sha2_iv.h"
#include "../src/wyprimes.h"

#include <cstdint>
#include <err.h>
#include <immintrin.h>
#include <stdlib.h>
#include <unistd.h>

[[nodiscard]] static inline auto
add_epu64(__m128i a, __m128i b)
{
    return _mm_add_epi64(a, b);
}

[[nodiscard]] static inline auto
add_epu64(__m256i a, __m256i b)
{
    return _mm256_add_epi64(a, b);
}

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
    const auto key_mask = _mm_set_epi64x((int64_t)SHA_512_H0_1, (int64_t)SHA_512_H0_0);

    const auto swapped = _mm_shuffle_epi32(key, _MM_SHUFFLE(1, 0, 3, 2));

    // all ones if the lanes are equal, all zeros otherwise
    const auto equal_mask = _mm_cmpeq_epi64(key, swapped);

    key = _mm_xor_si128(key, _mm_and_si128(equal_mask, key_mask));

    return key;
}

/// Adjust \a key so that the two 64-bit lanes of each 128-bit half differ
/**
* The two 64-bit lanes of each 128-bit half of the key must differ.
* With a key half of (K, K), if the counter half (A, B) gives the output (X, Y),
* then the counter half (B, A) gives the output (Y, X).
* The shuffle and compare below work within each 128-bit half, so they check each
* half on its own.
*/
[[nodiscard]] static inline auto
rectify_key(__m256i key)
{
    // most significant elem first
    const auto key_mask = _mm256_set_epi64x((int64_t)SHA_512_H0_3, (int64_t)SHA_512_H0_2,
                                            (int64_t)SHA_512_H0_1, (int64_t)SHA_512_H0_0);

    const auto swapped = _mm256_shuffle_epi32(key, _MM_SHUFFLE(1, 0, 3, 2));

    // all ones if the lanes are equal, all zeros otherwise
    const auto equal_mask = _mm256_cmpeq_epi64(key, swapped);

    key = _mm256_xor_si256(key, _mm256_and_si256(equal_mask, key_mask));

    return key;
}

template <typename T>
[[nodiscard]] inline auto
get_inc();

template <>
[[nodiscard]] inline auto
get_inc<__m128i>()
{
    return wyprimes_vec128_01();
}

template <>
[[nodiscard]] inline auto
get_inc<__m256i>()
{
    return wyprimes_vec256();
}

template <typename T>
[[nodiscard]] consteval int
get_default_num_keys();

template <>
[[nodiscard]] consteval int
get_default_num_keys<__m128i>()
{
    return DEFAULT_AES_CTR_128_PRNG_NUM_KEYS;
}

template <>
[[nodiscard]] consteval int
get_default_num_keys<__m256i>()
{
    return DEFAULT_AES_CTR_256_PRNG_NUM_KEYS;
}

template <typename T>
[[nodiscard]] consteval int
get_default_num_rounds_per_key();

template <>
[[nodiscard]] consteval int
get_default_num_rounds_per_key<__m128i>()
{
    return DEFAULT_AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY;
}

template <>
[[nodiscard]] consteval int
get_default_num_rounds_per_key<__m256i>()
{
    return DEFAULT_AES_CTR_256_PRNG_NUM_ROUNDS_PER_KEY;
}

/// A PRNG that uses AES or VAES instructions
/**
* \tparam T the block type
* \tparam enc if \c true, use AES encryption, otherwise AES decryption
* \tparam dm if \c true, use the Davies-Meyer single-block-length compression function (in addition to AES encryption/decryption) to get the next PRNG output
* \tparam Nk the number of independent AES keys
* \tparam Nr the number of AES enc/dec rounds applied per key
*/
template <typename T, bool enc, bool dm, int Nk, int Nr>
struct aes_ctr_prng
{
    static_assert(Nk >= 1);
    static_assert(Nr >= 1);
    static_assert(Nk * Nr >= 3, "must do at least 3 rounds of AES enc/dec");

    using block_t = T;

private:
    block_t keys[Nk]; ///< The round keys
    block_t ctr; ///< The state/counter

public:
    /// Construct a PRNG seeded via \c getentropy.
    /**
    * \note This function terminates the calling process upon catastrophic error.
    */
    aes_ctr_prng()
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

    /// Fill \a dst with \a n PRNG outputs via AES encryption or decryption
    /**
    * \param dst the destination blocks
    * \param n the number of blocks to fill
    *
    * The counter is kept in a local variable, because the stores to \a dst could alias
    * \c this->ctr.  Without it, the compiler reloads and stores the counter on every
    * iteration of a loop it does not fully unroll.
    *
    * The keys are read through \c this on purpose.  A local copy of them could be spilled
    * to the stack, where nothing wipes it.
    *
    * The counter increment \c inc used below forms a Weyl sequence.
    * Criteria for its 64-bit lane values:
    *   1) Must be odd
    *   2) Must be unique across lanes
    *
    * \sa https://en.wikipedia.org/wiki/Weyl_sequence#In_computing
    */
    void fill(block_t* dst, const int n) noexcept
    {
        const auto inc = get_inc<block_t>();

        auto ctr = this->ctr;

        for (int i = 0; i < n; ++i)
        {
            if constexpr (enc)
            {
                if constexpr (dm)
                    dst[i] = aes_enc_davies_meyer(ctr, this->keys, Nk, Nr);
                else
                    dst[i] = aes_enc(ctr, this->keys, Nk, Nr);
            }
            else
            {
                if constexpr (dm)
                    dst[i] = aes_dec_davies_meyer(ctr, this->keys, Nk, Nr);
                else
                    dst[i] = aes_dec(ctr, this->keys, Nk, Nr);
            }

            ctr = add_epu64(ctr, inc);
        }

        this->ctr = ctr;
    }
};
