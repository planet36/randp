// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// AES-128 utilities
/**
* \file
* \author Steven Ward
*/

#pragma once

#if defined(__x86_64__) && defined(__AES__) && defined(__SSE4_1__)

#include "wyprimes.h"

#include <immintrin.h>

#if defined(__cplusplus)
extern "C" {
#endif

[[nodiscard]] static inline __m128i
wyprimes_vec128_01()
{
    // most significant elem first
    return _mm_set_epi64x((int64_t)WYPRIME1, (int64_t)WYPRIME0);
}

[[nodiscard]] static inline __m128i
wyprimes_vec128_23()
{
    // most significant elem first
    return _mm_set_epi64x((int64_t)WYPRIME3, (int64_t)WYPRIME2);
}

/// Adjust \a key so that its 64-bit lanes differ
/**
* The 64-bit lanes of the key must differ.
* With a key of (K, K), if the counter
* (A, B) gives the output (X, Y), then the counter (B, A) gives the output (Y, X).
* \param key the key
* \return the adjusted key
*/
[[nodiscard]] static inline __m128i
rectify_key_128(__m128i key)
{
    // SHA-512 H0[1] and H0[0] (FIPS 180-4, section 5.3.5)
    // most significant elem first
    const __m128i key_mask = _mm_set_epi64x((int64_t)UINT64_C(0xbb67ae8584caa73b),
                                            (int64_t)UINT64_C(0x6a09e667f3bcc908));

    const __m128i swapped = _mm_shuffle_epi32(key, _MM_SHUFFLE(1, 0, 3, 2));

    // all ones if the lanes are equal, all zeros otherwise
    const __m128i equal_mask = _mm_cmpeq_epi64(key, swapped);

    key = _mm_xor_si128(key, _mm_and_si128(equal_mask, key_mask));

    return key;
}

/// Do \c _mm_aesenc_si128 on data \a a with \a Nk keys \a keys \a Nr times for each key
/**
* \pre \a Nk must be at least \c 1.
* \pre \a Nr must be at least \c 1.
* \param a the data
* \param keys the keys
* \param Nk the number of \a keys
* \param Nr the number of rounds of encryption to perform for each key
* \return the encrypted result
*/
[[nodiscard]] static inline __m128i
aes_enc_128(__m128i a, const __m128i* keys, const int Nk, const int Nr)
{
    for (int k = 0; k < Nk; ++k)
    {
        for (int r = 0; r < Nr; ++r)
        {
            a = _mm_aesenc_si128(a, keys[k]);
        }
    }
    return a;
}

/// Do \c _mm_aesdec_si128 on data \a a with \a Nk keys \a keys \a Nr times for each key
/**
* \pre \a Nk must be at least \c 1.
* \pre \a Nr must be at least \c 1.
* \param a the data
* \param keys the keys
* \param Nk the number of \a keys
* \param Nr the number of rounds of decryption to perform for each key
* \return the decrypted result
*/
[[nodiscard]] static inline __m128i
aes_dec_128(__m128i a, const __m128i* keys, const int Nk, const int Nr)
{
    for (int k = 0; k < Nk; ++k)
    {
        for (int r = 0; r < Nr; ++r)
        {
            a = _mm_aesdec_si128(a, keys[k]);
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
[[nodiscard]] static inline __m128i
aes_enc_davies_meyer_128(const __m128i H, const __m128i* keys, const int Nk, const int Nr)
{
    return _mm_xor_si128(aes_enc_128(H, keys, Nk, Nr), H);
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
[[nodiscard]] static inline __m128i
aes_dec_davies_meyer_128(const __m128i H, const __m128i* keys, const int Nk, const int Nr)
{
    return _mm_xor_si128(aes_dec_128(H, keys, Nk, Nr), H);
}

#if defined(__cplusplus)
} // extern "C"
#endif

#if defined(__cplusplus)
// Function overloads for C++

[[nodiscard]] static inline __m128i
aes_enc(__m128i a, const __m128i* keys, const int Nk, const int Nr)
{
    return aes_enc_128(a, keys, Nk, Nr);
}

[[nodiscard]] static inline __m128i
aes_dec(__m128i a, const __m128i* keys, const int Nk, const int Nr)
{
    return aes_dec_128(a, keys, Nk, Nr);
}

[[nodiscard]] static inline __m128i
aes_enc_davies_meyer(const __m128i H, const __m128i* keys, const int Nk, const int Nr)
{
    return aes_enc_davies_meyer_128(H, keys, Nk, Nr);
}

[[nodiscard]] static inline __m128i
aes_dec_davies_meyer(const __m128i H, const __m128i* keys, const int Nk, const int Nr)
{
    return aes_dec_davies_meyer_128(H, keys, Nk, Nr);
}

#endif

#endif
