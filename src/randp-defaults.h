// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// Default values for AES-CTR Random Pool (randp)
/**
* \file
* \author Steven Ward
*/

#pragma once

/// If \c true, use AES encryption, otherwise AES decryption
constexpr bool DEFAULT_RANDP_PRNG_USE_ENC = true;

/// If \c true, use the Davies-Meyer single-block-length compression function
/// (in addition to AES encryption/decryption) to get the next PRNG output
constexpr bool DEFAULT_RANDP_PRNG_USE_DAVIES_MEYER = false;

/// The number of bytes in the pool when the PRNG is AES-CTR-128
/**
* This must be a positive multiple of 32.
*/
constexpr int DEFAULT_RANDP_128_POOL_SIZE_BYTES = 256;

/// The number of bytes in the pool when the PRNG is AES-CTR-256 (VAES)
/**
* This must be a positive multiple of 32.
*/
constexpr int DEFAULT_RANDP_256_POOL_SIZE_BYTES = 512;

/// The number of pool regenerations between reseeds when the PRNG is AES-CTR-128
/**
* # [NIST Special Publication 800-90A Revision 1](https://csrc.nist.gov/pubs/sp/800/90/a/r1/final)
*
* ### 10.2.1 CTR_DRBG
*
* ##### Table 3: Definitions for the CTR_DRBG
*
* |                                                                | AES-128 |
* |----------------------------------------------------------------|---------|
* | Maximum number of requests between reseeds (*reseed_interval*) |    2^48 |
*/
constexpr int DEFAULT_RANDP_128_RESEED_INTERVAL = (1 << 10);

/// The number of pool regenerations between reseeds when the PRNG is AES-CTR-256 (VAES)
/**
* The interval counts refills, not bytes.  This one is derived from the AES-CTR-128
* defaults so that both PRNGs generate the same number of bytes between reseeds.
*/
constexpr int DEFAULT_RANDP_256_RESEED_INTERVAL =
    DEFAULT_RANDP_128_RESEED_INTERVAL * DEFAULT_RANDP_128_POOL_SIZE_BYTES /
    DEFAULT_RANDP_256_POOL_SIZE_BYTES;

static_assert(DEFAULT_RANDP_256_RESEED_INTERVAL * DEFAULT_RANDP_256_POOL_SIZE_BYTES ==
              DEFAULT_RANDP_128_RESEED_INTERVAL * DEFAULT_RANDP_128_POOL_SIZE_BYTES,
              "both PRNGs must generate the same number of bytes between reseeds");
