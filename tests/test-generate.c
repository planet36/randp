// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

/**
* This program will fill a buffer with random bytes and print it (in
* hexadecimal) to stdout.
*/

#include "../arp.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	void (*func_ptr)(void* buf, size_t n) = arp_bytes;

	for (int i = 1; i < argc; ++i)
	{
		const char* arg = argv[i];

		if (strcmp(arg, "arc4random") == 0)
		{
			func_ptr = arc4random_buf;
			break;
		}
	}

	constexpr size_t buf_size = 60;

	uint8_t* buf = (uint8_t*)calloc(buf_size, sizeof(uint8_t));
	assert(buf != nullptr);

	func_ptr(buf, buf_size);

	for (size_t i = 0; i < buf_size; ++i)
	{
		printf("%02X", buf[i]);
	}
	(void)putchar('\n');

	return 0;
}
