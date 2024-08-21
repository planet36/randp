// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

/**
* This program will dump random bytes to stdout (as long as stdout does not
* refer to a terminal), which could be analyzed by PractRand.
*/

#include "../arp.h"

#include <assert.h>
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
	if (isatty(STDOUT_FILENO)) // stdout is open and refers to a terminal.
	{
		// Do not write to a terminal.
		return 0;
	}

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

	uint8_t buf[4096] = {0};
	static_assert(sizeof(buf) % sizeof(uint32_t) == 0);

	do
	{
		func_ptr(buf, sizeof(buf));
	}
	while (write(STDOUT_FILENO, buf, sizeof(buf)) == sizeof(buf));

	return 0;
}
