// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// AES-256 utilities
/**
* \file
* \author Steven Ward
*
* The 256 in the name is the vector width (VAES), not the AES key size.
*/

#pragma once

#if defined(__x86_64__) && defined(__VAES__) && defined(__AVX2__)

#include "wyprimes.h"

#include <immintrin.h>

#if defined(__cplusplus)
extern "C" {
#endif

[[nodiscard]] static inline __m256i
wyprimes_vec256()
{
    // most significant elem first
    return _mm256_set_epi64x((int64_t)WYPRIME3, (int64_t)WYPRIME2,
                             (int64_t)WYPRIME1, (int64_t)WYPRIME0);
}

/// Adjust \a key so that the two 64-bit lanes of each 128-bit half differ
/**
* The two 64-bit lanes of each 128-bit half of the key must differ.
* With a key half of (K, K), if the counter half (A, B) gives the output (X, Y),
* then the counter half (B, A) gives the output (Y, X).
* The shuffle and compare below work within each 128-bit half, so they check each
* half on its own.
* \param key the key
* \return the adjusted key
*/
[[nodiscard]] static inline __m256i
rectify_key_256(__m256i key)
{
    // SHA-512 H0[3] through H0[0] (FIPS 180-4, section 5.3.5)
    // most significant elem first
    const __m256i key_mask = _mm256_set_epi64x((int64_t)UINT64_C(0xa54ff53a5f1d36f1),
                                               (int64_t)UINT64_C(0x3c6ef372fe94f82b),
                                               (int64_t)UINT64_C(0xbb67ae8584caa73b),
                                               (int64_t)UINT64_C(0x6a09e667f3bcc908));

    const __m256i swapped = _mm256_shuffle_epi32(key, _MM_SHUFFLE(1, 0, 3, 2));

    // all ones if the lanes are equal, all zeros otherwise
    const __m256i equal_mask = _mm256_cmpeq_epi64(key, swapped);

    key = _mm256_xor_si256(key, _mm256_and_si256(equal_mask, key_mask));

    return key;
}

/// Do \c _mm256_aesenc_epi128 on data \a a with \a Nk keys \a keys \a Nr times for each key
/**
* \pre \a Nk must be at least \c 1.
* \pre \a Nr must be at least \c 1.
* \param a the data
* \param keys the keys
* \param Nk the number of \a keys
* \param Nr the number of rounds of encryption to perform for each key
* \return the encrypted result
*/
[[nodiscard]] static inline __m256i
aes_enc_256(__m256i a, const __m256i* keys, const int Nk, const int Nr)
{
    for (int k = 0; k < Nk; ++k)
    {
        for (int r = 0; r < Nr; ++r)
        {
            a = _mm256_aesenc_epi128(a, keys[k]);
        }
    }
    return a;
}

/// Do \c _mm256_aesdec_epi128 on data \a a with \a Nk keys \a keys \a Nr times for each key
/**
* \pre \a Nk must be at least \c 1.
* \pre \a Nr must be at least \c 1.
* \param a the data
* \param keys the keys
* \param Nk the number of \a keys
* \param Nr the number of rounds of decryption to perform for each key
* \return the decrypted result
*/
[[nodiscard]] static inline __m256i
aes_dec_256(__m256i a, const __m256i* keys, const int Nk, const int Nr)
{
    for (int k = 0; k < Nk; ++k)
    {
        for (int r = 0; r < Nr; ++r)
        {
            a = _mm256_aesdec_epi128(a, keys[k]);
        }
    }
    return a;
}

/// Davies-Meyer single-block-length compression function that uses AES as the block cipher
/**
* \sa https://en.wikipedia.org/wiki/One-way_compression_function#Davies%E2%80%93Meyer
* \pre \a Nk must be at least \c 1.
* \pre \a Nr must be at least \c 1.
* \param H the previous hash value
* \param keys the keys
* \param Nk the number of \a keys
* \param Nr the number of rounds of encryption to perform for each key
* \return the compressed result
*/
[[nodiscard]] static inline __m256i
aes_enc_davies_meyer_256(const __m256i H, const __m256i* keys, const int Nk, const int Nr)
{
    return _mm256_xor_si256(aes_enc_256(H, keys, Nk, Nr), H);
}

/// Davies-Meyer single-block-length compression function that uses AES as the block cipher
/**
* \sa https://en.wikipedia.org/wiki/One-way_compression_function#Davies%E2%80%93Meyer
* \pre \a Nk must be at least \c 1.
* \pre \a Nr must be at least \c 1.
* \param H the previous hash value
* \param keys the keys
* \param Nk the number of \a keys
* \param Nr the number of rounds of decryption to perform for each key
* \return the compressed result
*/
[[nodiscard]] static inline __m256i
aes_dec_davies_meyer_256(const __m256i H, const __m256i* keys, const int Nk, const int Nr)
{
    return _mm256_xor_si256(aes_dec_256(H, keys, Nk, Nr), H);
}

#if defined(__cplusplus)
} // extern "C"
#endif

#if defined(__cplusplus)
// Function overloads for C++

[[nodiscard]] static inline __m256i
aes_enc(__m256i a, const __m256i* keys, const int Nk, const int Nr)
{
    return aes_enc_256(a, keys, Nk, Nr);
}

[[nodiscard]] static inline __m256i
aes_dec(__m256i a, const __m256i* keys, const int Nk, const int Nr)
{
    return aes_dec_256(a, keys, Nk, Nr);
}

[[nodiscard]] static inline __m256i
aes_enc_davies_meyer(const __m256i H, const __m256i* keys, const int Nk, const int Nr)
{
    return aes_enc_davies_meyer_256(H, keys, Nk, Nr);
}

[[nodiscard]] static inline __m256i
aes_dec_davies_meyer(const __m256i H, const __m256i* keys, const int Nk, const int Nr)
{
    return aes_dec_davies_meyer_256(H, keys, Nk, Nr);
}

#endif

#endif
