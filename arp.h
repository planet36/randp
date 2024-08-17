// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

/// AES Random Pool (ARP)
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

/// Fill a buffer with \a n random bytes
/**
* This is similar to \c arc4random_buf.
*/
void arp_bytes(void* buf, size_t n);

/// Return a uniform random unsigned integer
/**
* This is similar to \c arc4random.
*/
uint8_t arp_u8();
uint16_t arp_u16();
uint32_t arp_u32();
uint64_t arp_u64();

/// Return a uniform random unsigned integer less than \a upper_bound
/**
* This is similar to \c arc4random_uniform.
* If \a upper_bound is \c 0, it's treated as unbounded.
*/
uint32_t arp_lt_u32(uint32_t upper_bound);

#ifdef __cplusplus
}
#endif
