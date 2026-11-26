// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// Default values for AES-CTR-256 PRNG
/**
* \file
* \author Steven Ward
*
* The 256 in the name is the vector width (VAES), not the AES key size.
*
* The number of keys must be at least 3 and at most 7.  The maximum keeps the state within the
* 256 bytes that getentropy allows.
*/

#pragma once

/// The number of independent AES keys, which is the number of AES enc/dec rounds
/**
* Each key is the round key of exactly one round.  Applying a key in more than one round
* would make those rounds the same permutation, which is the self-similarity that slide
* attacks exploit.
*/
constexpr int DEFAULT_AES_CTR_256_PRNG_NUM_KEYS = 3;
