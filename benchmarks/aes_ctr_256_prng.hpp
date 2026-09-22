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
* 1. different values of \c Nk and \c Nr
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

[[nodiscard]] static inline auto
add_epu64(__m256i a, __m256i b)
{
    return _mm256_add_epi64(a, b);
}

/// A PRNG that uses VAES instructions
/**
* \tparam enc if \c true, use AES encryption, otherwise AES decryption
* \tparam dm if \c true, use the Davies-Meyer single-block-length compression function (in addition to AES encryption/decryption) to get the next PRNG output
* \tparam Nk the number of independent AES keys
* \tparam Nr the number of AES enc/dec rounds applied per key
*
* Each output is two 128-bit AES blocks, one in each 128-bit half of a \c __m256i.
* VAES encrypts each half independently, with the matching half of each key as its round key.
* The 256 in the name is the vector width, not the AES key size.
*/
template <bool enc, bool dm, int Nk, int Nr>
struct aes_ctr_256_prng
{
    static_assert(Nk >= 1);
    static_assert(Nr >= 1);
    static_assert(Nk * Nr >= 3, "must do at least 3 rounds of AES enc/dec");

    using block_t = __m256i;

private:
    block_t keys[Nk]; ///< The round keys
    block_t ctr; ///< The state/counter

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
        for (int i = 0; i < Nk; ++i)
        {
            // The two 64-bit lanes of each 128-bit half of the key must differ.
            // With a key half of (K, K), if the counter half (A, B) gives the output (X, Y),
            // then the counter half (B, A) gives the output (Y, X).
            // The shuffle and compare below work within each 128-bit half, so they check each
            // half on its own.

            // most significant elem first
            const block_t key_mask = _mm256_set_epi64x((int64_t)SHA_512_H0_3,
                    (int64_t)SHA_512_H0_2, (int64_t)SHA_512_H0_1, (int64_t)SHA_512_H0_0);

            const block_t swapped = _mm256_shuffle_epi32(this->keys[i], _MM_SHUFFLE(1, 0, 3, 2));
            // all ones if the lanes are equal, all zeros otherwise
            const block_t equal_mask = _mm256_cmpeq_epi64(this->keys[i], swapped);

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
    [[nodiscard]] block_t next() noexcept
    {
        const block_t inc = wyprimes_vec256();

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
