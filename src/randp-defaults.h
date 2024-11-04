// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

/// Default values for AES-CTR Random Pool (randp)
/**
\file
\author Steven Ward
*/

#pragma once

#define DEFAULT_RANDP_PRNG_USE_ENC true

#define DEFAULT_RANDP_PRNG_USE_DAVIES_MEYER false

#define DEFAULT_RANDP_NUM_BLOCKS 0x10

/**
# [NIST Special Publication 800-90A Revision 1](https://csrc.nist.gov/pubs/sp/800/90/a/r1/final)
### 10.2.1 CTR_DRBG
##### Table 3: Definitions for the CTR_DRBG
|                                                                | AES-128 |
|----------------------------------------------------------------|---------|
| Maximum number of requests between reseeds (*reseed_interval*) |    2^48 |
*/
#define DEFAULT_RANDP_RESEED_COUNTDOWN_MIN (1 << 10)
