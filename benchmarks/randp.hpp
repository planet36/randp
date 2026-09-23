// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// AES-CTR Random Pool (randp) (This C++ implementation is used only for benchmarking.)
/**
* \file
* \author Steven Ward
* Inspired by \c arc4random.
* \sa https://github.com/openbsd/src/blob/master/lib/libc/crypt/arc4random.c
* \sa https://man7.org/linux/man-pages/man3/arc4random.3.html
*
* The raisons d'etre of this class are to test
* 1. different values of \c RANDP_POOL_SIZE_BYTES and \c RANDP_RESEED_INTERVAL
* 2. using a mutex for the static randp data instead of a \c thread_local instance
*/

#pragma once

#include "../src/allocate.h"
#include "../src/randp-defaults.h"
#include "aes_ctr_prng.hpp"

#include <immintrin.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <sys/user.h>
#include <threads.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#if defined(__x86_64__) && defined(__VAES__) && defined(__AVX2__)

#define RANDP_BLOCK_TYPE __m256i

#elif defined(__x86_64__) && defined(__AES__) && defined(__SSE4_1__)

#define RANDP_BLOCK_TYPE __m128i

#else
#error "Architecture not supported"
#endif

/// A pool of random bytes
/**
* \tparam RANDP_POOL_SIZE_BYTES the number of bytes in the pool
* \tparam RANDP_RESEED_INTERVAL the number of pool regenerations between reseeds
* \tparam T the block type: \c __m128i for AES, or \c __m256i for VAES
* \tparam enc if \c true, use AES encryption, otherwise AES decryption
* \tparam dm if \c true, use the Davies-Meyer single-block-length compression function (in addition to AES encryption/decryption) to get the next PRNG output
* \tparam Nk the number of independent AES keys
* \tparam Nr the number of AES enc/dec rounds applied per key
*/
template <
    int RANDP_POOL_SIZE_BYTES = DEFAULT_RANDP_POOL_SIZE_BYTES,
    int RANDP_RESEED_INTERVAL = DEFAULT_RANDP_RESEED_INTERVAL,
    // {{{ PRNG params
    typename T = RANDP_BLOCK_TYPE,
    bool enc = DEFAULT_RANDP_PRNG_USE_ENC,
    bool dm = DEFAULT_RANDP_PRNG_USE_DAVIES_MEYER,
    int Nk = get_default_num_keys<T>(),
    int Nr = get_default_num_rounds_per_key<T>()
    // }}}
>
struct randp
{
    static_assert(RANDP_POOL_SIZE_BYTES > 0, "randp pool byte size must be positive");
    static_assert((RANDP_POOL_SIZE_BYTES % 32) == 0, "randp pool byte size must be a multiple of 32");

    static_assert(RANDP_RESEED_INTERVAL >= 1, "randp reseed interval must be positive");

    aes_ctr_prng<T, enc, dm, Nk, Nr> prng;
    uint8_t pool[RANDP_POOL_SIZE_BYTES];
    int reseed_countdown;     ///< The PRNG is reseeded when this is 0.
    int rand_bytes_remaining; ///< The pool is regenerated when this is 0.

    /// Regenerate the pool, reseeding the PRNG first if the reseed countdown has reached zero.
    void regen()
    {
        if (this->reseed_countdown == 0)
        {
            prng.reseed();
            this->reseed_countdown = RANDP_RESEED_INTERVAL;
        }

        auto* blocks = (T*)(&this->pool[0]);

        constexpr int num_blocks = RANDP_POOL_SIZE_BYTES / sizeof(T);

        for (int i = 0; i < num_blocks; ++i)
        {
            blocks[i] = prng.next();
        }

        this->rand_bytes_remaining = RANDP_POOL_SIZE_BYTES;
        --this->reseed_countdown;
    }

    /// Deallocate a thread's \c randp pool when the thread exits.
    /**
    * \param p the randp pool, as passed to \c tss_set
    */
    static void
    destroy(void* p) noexcept
    {
        deallocate(p, sizeof(randp));
    }

    static inline tss_t tss_key;
    static inline once_flag tss_once = ONCE_FLAG_INIT;

    static void
    tss_key_init() noexcept
    {
        if (tss_create(&tss_key, destroy) != thrd_success)
            errx(EXIT_FAILURE, "tss_create failed");
    }
};

/// Fill a buffer with random bytes using a \c thread_local pool.
/**
* \param buf the destination buffer
* \param n the number of bytes to fill
* \pre \a buf is not null
* \pre \a buf is at least \a n bytes in size
*/
template <
    int RANDP_POOL_SIZE_BYTES = DEFAULT_RANDP_POOL_SIZE_BYTES,
    int RANDP_RESEED_INTERVAL = DEFAULT_RANDP_RESEED_INTERVAL,
    // {{{ PRNG params
    typename T = RANDP_BLOCK_TYPE,
    bool enc = DEFAULT_RANDP_PRNG_USE_ENC,
    bool dm = DEFAULT_RANDP_PRNG_USE_DAVIES_MEYER,
    int Nk = get_default_num_keys<T>(),
    int Nr = get_default_num_rounds_per_key<T>()
    // }}}
>
void
randp_bytes(void* buf, size_t n) noexcept [[gnu::nonnull]]
{
    using randp_t = randp<RANDP_POOL_SIZE_BYTES, RANDP_RESEED_INTERVAL, T, enc, dm, Nk, Nr>;

    static thread_local randp_t* this_ = nullptr;

    static_assert(alignof(randp_t) == alignof(T),
                  "randp must have the alignment of its block type");

    static_assert(offsetof(randp_t, pool) % sizeof(T) == 0,
                  "randp pool must start on a block boundary");

    static_assert(sizeof(randp_t) <= PAGE_SIZE, "randp must fit in one page");

    if (this_ == nullptr)
    {
#if defined(__cplusplus)
        this_ = (decltype(this_))allocate(sizeof(*this_));
#else
        // Unreachable in C++.  Retained only to match randp.c.
        this_ = (typeof(this_))allocate(sizeof(*this_));
#endif
        call_once(&randp_t::tss_once, randp_t::tss_key_init);
        if (tss_set(randp_t::tss_key, this_) != thrd_success)
            errx(EXIT_FAILURE, "tss_set failed");
    }

    uint8_t* dst = (uint8_t*)buf;

    while (n > 0)
    {
        if (this_->rand_bytes_remaining == 0)
            this_->regen();

        uint8_t* src = &this_->pool[RANDP_POOL_SIZE_BYTES - this_->rand_bytes_remaining];

        const int m = (int)MIN(n, (size_t)this_->rand_bytes_remaining);

        (void)memcpy(dst, src, m);
        explicit_bzero(src, m);

        dst += m;
        this_->rand_bytes_remaining -= m;
        n -= m;
    }
}

// {{{ This uses a global mutex instead of thread_local keyword for the randp*.

#include <pthread.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
static pthread_mutex_t randp_mtx = PTHREAD_MUTEX_INITIALIZER;
#pragma GCC diagnostic pop

/// Fill a buffer with random bytes using a pool shared across all threads, protected by a mutex.
/**
* \param buf the destination buffer
* \param n the number of bytes to fill
* \pre \a buf is not null
* \pre \a buf is at least \a n bytes in size
*/
template <
    int RANDP_POOL_SIZE_BYTES = DEFAULT_RANDP_POOL_SIZE_BYTES,
    int RANDP_RESEED_INTERVAL = DEFAULT_RANDP_RESEED_INTERVAL,
    // {{{ PRNG params
    typename T = RANDP_BLOCK_TYPE,
    bool enc = DEFAULT_RANDP_PRNG_USE_ENC,
    bool dm = DEFAULT_RANDP_PRNG_USE_DAVIES_MEYER,
    int Nk = get_default_num_keys<T>(),
    int Nr = get_default_num_rounds_per_key<T>()
    // }}}
>
void
randp_bytes_MUTEX(void* buf, size_t n) noexcept [[gnu::nonnull]]
{
    using randp_t = randp<RANDP_POOL_SIZE_BYTES, RANDP_RESEED_INTERVAL, T, enc, dm, Nk, Nr>;

    // Intentionally not thread_local
    static randp_t* this_ = nullptr;

    static_assert(alignof(randp_t) == alignof(T),
                  "randp must have the alignment of its block type");

    static_assert(offsetof(randp_t, pool) % sizeof(T) == 0,
                  "randp pool must start on a block boundary");

    static_assert(sizeof(randp_t) <= PAGE_SIZE, "randp must fit in one page");

    (void)pthread_mutex_lock(&randp_mtx);

    if (this_ == nullptr)
    {
#if defined(__cplusplus)
        this_ = (decltype(this_))allocate(sizeof(*this_));
#else
        // Unreachable in C++.  Retained only to match randp.c.
        this_ = (typeof(this_))allocate(sizeof(*this_));
#endif
        // Do not deallocate this_ when the thread exits.
        // It's not thread_local, unlike the randp_bytes function, so there's only 1 instance
        // per template instantiation, shared across all threads.
    }

    uint8_t* dst = (uint8_t*)buf;

    while (n > 0)
    {
        if (this_->rand_bytes_remaining == 0)
            this_->regen();

        uint8_t* src = &this_->pool[RANDP_POOL_SIZE_BYTES - this_->rand_bytes_remaining];

        const int m = (int)MIN(n, (size_t)this_->rand_bytes_remaining);

        (void)memcpy(dst, src, m);
        explicit_bzero(src, m);

        dst += m;
        this_->rand_bytes_remaining -= m;
        n -= m;
    }

    (void)pthread_mutex_unlock(&randp_mtx);
}

// }}}

#undef MIN
#undef RANDP_BLOCK_TYPE
