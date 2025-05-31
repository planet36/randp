// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

/// AES-CTR Random Pool (randp)
/**
\file
\author Steven Ward
Inspired by \c arc4random.
\sa https://github.com/openbsd/src/blob/master/lib/libc/crypt/arc4random.c
\sa https://man7.org/linux/man-pages/man3/arc4random.3.html
*/

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Public API
/**
* <code>void randp_bytes(void* buf, size_t n)</code>
* Fill a buffer with \a n random bytes
* This is similar to \c arc4random_buf.
*
* <code>uint8_t randp_u8();</code>
* <code>uint16_t randp_u16();</code>
* <code>uint32_t randp_u32();</code>
* <code>uint64_t randp_u64();</code>
* Return a uniform random unsigned integer
* This is similar to \c arc4random.
*
* <code>uint32_t randp_lt_u32(uint32_t upper_bound);</code>
* Return a uniform random unsigned integer less than \a upper_bound
* This is similar to \c arc4random_uniform.
* If \a upper_bound is \c 0, it's treated as unbounded.
*/

void randp_bytes(void* buf, size_t n);

uint8_t randp_u8();

uint16_t randp_u16();

uint32_t randp_u32();

uint64_t randp_u64();

uint32_t randp_lt_u32(uint32_t upper_bound);

#ifdef __cplusplus
}
#endif
