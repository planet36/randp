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

// XXX: n must be a multiple of sizeof(uint32_t)
void
mrand48_wrapper(void* buf, size_t n)
{
	const size_t num_elems = n / sizeof(uint32_t);
	uint32_t* dst = buf;
	for (size_t i = 0; i < num_elems; ++i)
	{
		dst[i] = (uint32_t)mrand48();
	}
}

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
		if (strcmp(arg, "mrand48") == 0)
		{
			unsigned short seed16v[3];
			assert(getentropy(seed16v, sizeof(seed16v)) == 0);
			(void)seed48(seed16v);
			func_ptr = mrand48_wrapper;
			break;
		}
	}

	constexpr size_t buf_size = 60;
	assert(buf_size % sizeof(uint32_t) == 0);

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
