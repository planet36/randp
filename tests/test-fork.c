// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

/**
* This program will initialize a pool of random data, then it will fork
* itself. Then the parent and child each will print a random number. The two
* numbers will be different, proving that the AES Random Pool (ARP) is
* fork-safe.
* \sa https://wiki.openssl.org/index.php/Random_fork-safety
*/

#include "../arp.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int
main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	constexpr int max_len_func_name = 10;

	const char* func_name = "arp_u32";
	uint32_t (*func_ptr)(void) = arp_u32;

	for (int i = 1; i < argc; ++i)
	{
		const char* arg = argv[i];

		if (strcmp(arg, "arc4random") == 0)
		{
			func_name = "arc4random";
			func_ptr = arc4random;

			break;
		}
	}

	// allocate/initialize pools of random data before fork
	(void)func_ptr();

	const pid_t child_pid = fork();
	assert(child_pid >= 0);

	const bool is_child = (child_pid == 0);

	const char* pid_category_label;

	if (is_child)
	{
		pid_category_label = "[child] ";
		sleep(1);
	}
	else
	{
		pid_category_label = "[parent]";
	}

	printf("%-*s %s %08x\n", max_len_func_name, func_name,
	       pid_category_label, func_ptr());

	if (!is_child)
	{
		const pid_t w = waitpid(child_pid, nullptr, 0);
		assert(w != -1);
		assert(w == child_pid);
	}

	return 0;
}
