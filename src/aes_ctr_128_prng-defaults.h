// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// Default values for AES-CTR-128 PRNG
/**
* \file
* \author Steven Ward
*
* The number of keys must be at least 3.
*/

#pragma once

/// The number of independent AES keys, which is the number of AES enc/dec rounds
/**
* Each key is the round key of exactly one round.  Applying a key in more than one round
* would make those rounds the same permutation, which is the self-similarity that slide
* attacks exploit.
*/
constexpr int DEFAULT_AES_CTR_128_PRNG_NUM_KEYS = 3;
