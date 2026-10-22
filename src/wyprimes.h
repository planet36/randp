// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// wyhash & wyrand primes
/**
* \file
* \author Steven Ward
* \sa https://github.com/wangyi-fudan/wyhash/blob/master/wyhash.h#L144
*/

#pragma once

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

static constexpr uint64_t WYPRIME0 = UINT64_C(0x2d358dccaa6c78a5); // prime (popcount = 32)
static constexpr uint64_t WYPRIME1 = UINT64_C(0x8bb84b93962eacc9); // prime (popcount = 32)
static constexpr uint64_t WYPRIME2 = UINT64_C(0x4b33a62ed433d4a3); // prime (popcount = 32)
static constexpr uint64_t WYPRIME3 = UINT64_C(0x4d5a2da51de1aa47); // prime (popcount = 32)

static_assert((WYPRIME0 & 1) != 0, "must be odd");
static_assert((WYPRIME1 & 1) != 0, "must be odd");
static_assert((WYPRIME2 & 1) != 0, "must be odd");
static_assert((WYPRIME3 & 1) != 0, "must be odd");

static_assert(__builtin_popcountll(WYPRIME0) == 32, "popcount must be 32");
static_assert(__builtin_popcountll(WYPRIME1) == 32, "popcount must be 32");
static_assert(__builtin_popcountll(WYPRIME2) == 32, "popcount must be 32");
static_assert(__builtin_popcountll(WYPRIME3) == 32, "popcount must be 32");

#if defined(__cplusplus)
} // extern "C"
#endif
