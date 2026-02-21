// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/**
* This program includes the single header file and verifies that the public API
* can be called.
*/

#include "../randp-single.h"

[[gnu::optimize("O0")]]
int
main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    {
        [[maybe_unused]] uint8_t buf[16];
        randp_bytes(buf, sizeof(buf));
    }

    {
        [[maybe_unused]] volatile uint8_t result = randp_u8();
    }

    {
        [[maybe_unused]] volatile uint16_t result = randp_u16();
    }

    {
        [[maybe_unused]] volatile uint32_t result = randp_u32();
    }

    {
        [[maybe_unused]] volatile uint64_t result = randp_u64();
    }

    {
        const uint32_t upper_bound = 6;
        [[maybe_unused]] volatile uint32_t result = randp_lt_u32(upper_bound);
    }

    return 0;
}
