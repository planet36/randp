// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

/// Allocate private, anonymous memory using \c mmap
/**
\file
\author Steven Ward
\sa https://man7.org/linux/man-pages/man2/mmap.2.html
\sa https://man7.org/linux/man-pages/man2/madvise.2.html
\sa https://github.com/google/boringssl/blob/master/SANDBOXING.md
\sa https://github.com/aws/s2n-tls/blob/main/utils/s2n_fork_detection.c
*/

#pragma once

#include <err.h>
#include <stdlib.h>
#include <sys/mman.h>

#ifdef __cplusplus
extern "C" {
#endif

static void*
allocate(const size_t len)
{
	void* addr = mmap(nullptr, len, PROT_READ | PROT_WRITE,
	                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (addr == MAP_FAILED)
		err(EXIT_FAILURE, "mmap");

	if (madvise(addr, len, MADV_DONTDUMP) < 0)
		err(EXIT_FAILURE, "madvise");

	if (madvise(addr, len, MADV_WIPEONFORK) < 0)
		err(EXIT_FAILURE, "madvise");

	return addr;
}

#ifdef __cplusplus
}
#endif
