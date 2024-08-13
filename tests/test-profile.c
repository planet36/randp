// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

#include "arp.h"
#include "valgrind/callgrind.h"

#include <assert.h>
#include <stdlib.h>
//#include <unistd.h>

int
main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	constexpr size_t buf_size = 2048;

	uint8_t* buf = (uint8_t*)calloc(buf_size, sizeof(uint8_t));
	assert(buf != nullptr);

	CALLGRIND_START_INSTRUMENTATION;

	for (int i = 0; i < 100; ++i)
	{
		arp_copy_to(buf, buf_size);
		//arc4random_buf(buf, buf_size);
	}

	CALLGRIND_STOP_INSTRUMENTATION;

	return 0;
}
