// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

/// Daniel Lemire's "nearly divisionless" algorithm
/**
\file
\author Steven Ward
"Fast Random Integer Generation in an Interval" by Daniel Lemire
\sa https://arxiv.org/abs/1805.10941
\sa https://lemire.me/blog/2016/06/30/fast-random-shuffling/
\sa https://lemire.me/blog/2019/06/06/nearly-divisionless-random-integer-generation-on-various-systems/
\sa https://github.com/lemire/FastShuffleExperiments/blob/master/cpp/rangedrand.h
\sa https://github.com/lemire/Code-used-on-Daniel-Lemire-s-blog/blob/master/2019/06/06/rangedrand.h
\sa https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=libstdc%2B%2B-v3/include/bits/uniform_int_dist.h#l252
\sa https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=libstdc%2B%2B-v3/ChangeLog-2020;#l2655
\sa https://github.com/abseil/abseil-cpp/blob/master/absl/random/uniform_int_distribution.h#L207
*/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static uint32_t
random_bounded_nearlydivisionless32(const uint32_t s, uint32_t (*prng)())
{
    typedef uint32_t T;
    typedef uint64_t T2; // twice as wide
    static_assert(sizeof(T2) == 2 * sizeof(T));
    constexpr unsigned int bit_width = UINT32_WIDTH;

    T2 product;

    product = (T2)prng() * (T2)s; // in [0, s * 2^L)
    if ((T)product < s)
    {
        // 2^L mod s == (2^L - s) mod s == (-s) mod s
        const T threshold = (-s) % s; // 2^L mod s
        while ((T)product < threshold)
        {
            product = (T2)prng() * (T2)s; // in [0, s * 2^L)
        }
    }
    return product >> bit_width; // in [0, s)
}

#if defined(__SIZEOF_INT128__)
[[maybe_unused]] static uint64_t
random_bounded_nearlydivisionless64(const uint64_t s, uint64_t (*prng)())
{
    typedef uint64_t T;
    typedef __uint128_t T2; // twice as wide
    static_assert(sizeof(T2) == 2 * sizeof(T));
    constexpr unsigned int bit_width = UINT64_WIDTH;

    T2 product;

    product = (T2)prng() * (T2)s; // in [0, s * 2^L)
    if ((T)product < s)
    {
        // 2^L mod s == (2^L - s) mod s == (-s) mod s
        const T threshold = (-s) % s; // 2^L mod s
        while ((T)product < threshold)
        {
            product = (T2)prng() * (T2)s; // in [0, s * 2^L)
        }
    }
    return product >> bit_width; // in [0, s)
}
#endif

#ifdef __cplusplus
}
#endif
