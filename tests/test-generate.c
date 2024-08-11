// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

#include "arp.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	constexpr int max_len_func_name = 14;

	const char* func_name = "arp_copy_to";
	void (*func_ptr)(void* buf, size_t n) = arp_copy_to;

	for (int i = 1; i < argc; ++i)
	{
		const char* arg = argv[i];

		if (strcmp(arg, "arc4random") == 0)
		{
			func_name = "arc4random_buf";
			func_ptr = arc4random_buf;
			break;
		}
	}

	constexpr size_t buf_size = 60;

	uint8_t* buf = (uint8_t*)calloc(buf_size, sizeof(uint8_t));
	assert(buf != nullptr);

	func_ptr(buf, buf_size);

	printf("%-*s ", max_len_func_name, func_name);
	for (size_t i = 0; i < buf_size; ++i)
	{
		printf("%02X", buf[i]);
	}
	putchar('\n');

	return 0;
}
