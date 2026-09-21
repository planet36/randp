// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// Default values for AES-CTR-128 PRNG
/**
* \file
* \author Steven Ward
*
* The product of the number of keys and the number of rounds per key must be at least 3.
*/

#pragma once

/// The number of independent AES keys
constexpr int DEFAULT_AES_CTR_128_PRNG_NUM_KEYS = 3;

/// The number of AES enc/dec rounds applied per key
constexpr int DEFAULT_AES_CTR_128_PRNG_NUM_ROUNDS_PER_KEY = 1;
