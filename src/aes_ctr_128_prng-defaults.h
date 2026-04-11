// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// Default values for AES-CTR-128 PRNG
/**
* \file
* \author Steven Ward
*/

#pragma once

/// The number of independent AES keys
/**
* The product of \c AESCTR128_PRNG_NUM_KEYS and \c AESCTR128_PRNG_NUM_ROUNDS_PER_KEY
* must be at least \c 3.
*/
#define DEFAULT_AESCTR128_PRNG_NUM_KEYS 3

/// The number of AES enc/dec rounds applied per key
/**
* The product of \c AESCTR128_PRNG_NUM_KEYS and \c AESCTR128_PRNG_NUM_ROUNDS_PER_KEY
* must be at least \c 3.
*/
#define DEFAULT_AESCTR128_PRNG_NUM_ROUNDS_PER_KEY 1
