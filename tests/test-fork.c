// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: OSL-3.0

/**
* This program will initialize a pool of random bytes, then fork itself.
* After the fork, the parent and child processes will print a random number.
* The two numbers will be different, proving that the random number generator
* is fork-safe.
* \sa https://wiki.openssl.org/index.php/Random_fork-safety
*/

#include "../randp.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

uint32_t
mrand48_wrapper()
{
    return (uint32_t)mrand48();
}

int
main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    constexpr int max_len_func_name = 10;

    const char* func_name = "randp_u32";
    uint32_t (*func_ptr)(void) = randp_u32;

    for (int i = 1; i < argc; ++i)
    {
        const char* arg = argv[i];

        if (strcmp(arg, "arc4random") == 0)
        {
            func_name = "arc4random";
            func_ptr = arc4random;
            break;
        }
        if (strcmp(arg, "mrand48") == 0)
        {
            unsigned short seed16v[3];
            assert(getentropy(seed16v, sizeof(seed16v)) == 0);
            (void)seed48(seed16v);
            func_name = "mrand48";
            func_ptr = mrand48_wrapper;
            break;
        }
    }

    // allocate/initialize before fork
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

    printf("%-*s %s %08x\n", max_len_func_name, func_name, pid_category_label, func_ptr());

    if (!is_child)
    {
        const pid_t w = waitpid(child_pid, nullptr, 0);
        assert(w != -1);
        assert(w == child_pid);
    }

    return 0;
}
