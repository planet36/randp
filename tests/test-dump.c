// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

/**
* This program will dump random bytes to stdout (as long as stdout does not
* refer to a terminal).  Those bytes could be analyzed by PractRand.
*/

#include "arp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	if (isatty(STDOUT_FILENO)) // stdout is open and refers to a terminal.
	{
		// Do not write to a terminal.
		return 0;
	}

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

	(void)fputs(func_name, stderr);
	(void)fputc('\n', stderr);

	uint8_t buf[4096] = {0};

	do
	{
		func_ptr(buf, sizeof(buf));
	}
	while (write(STDOUT_FILENO, buf, sizeof(buf)) == sizeof(buf));

	return 0;
}