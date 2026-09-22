// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// Default values for AES-CTR Random Pool (randp)
/**
* \file
* \author Steven Ward
*/

#pragma once

/// If \c true, use AES encryption, otherwise AES decryption.
constexpr bool DEFAULT_RANDP_PRNG_USE_ENC = true;

/// If \c true, use the Davies-Meyer single-block-length compression function
/// (in addition to AES encryption/decryption) to get the next PRNG output.
constexpr bool DEFAULT_RANDP_PRNG_USE_DAVIES_MEYER = false;

/// The number of blocks in the pool
/**
* The size of each block is \c sizeof(__m128i) (i.e. \c 16).
*/
constexpr int DEFAULT_RANDP_POOL_NUM_BLOCKS = 16;

/// The number of pool regenerations between reseeds
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
constexpr int DEFAULT_RANDP_RESEED_INTERVAL = (1 << 10);
