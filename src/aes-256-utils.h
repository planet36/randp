// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// AES-256 utilities
/**
* \file
* \author Steven Ward
*/

#pragma once

#if defined(__x86_64__) && defined(__VAES__)

#include <immintrin.h>

#if defined(__cplusplus)
extern "C" {
#endif

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
aes_enc_davies_meyer_256(const __m256i H,
                        const __m256i* keys,
                        const int Nk,
                        const int Nr)
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
aes_dec_davies_meyer_256(const __m256i H,
                        const __m256i* keys,
                        const int Nk,
                        const int Nr)
{
    return _mm256_xor_si256(aes_dec_256(H, keys, Nk, Nr), H);
}

#if defined(__cplusplus)
} // extern "C"
#endif

#endif
