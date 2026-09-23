// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// AES-CTR Random Pool (randp)
/**
* \file
* \author Steven Ward
* Inspired by \c arc4random.
* \sa https://github.com/openbsd/src/blob/master/lib/libc/crypt/arc4random.c
* \sa https://man7.org/linux/man-pages/man3/arc4random.3.html
*/

#include "aes_ctr_128_prng.h"
#include "aes_ctr_256_prng.h"
#include "allocate.h"
#include "nearlydivisionless.h"
#include "randp-defaults.h"

#include <immintrin.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <sys/user.h>
#include <threads.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#if !defined(RANDP_PRNG_USE_ENC)
#define RANDP_PRNG_USE_ENC DEFAULT_RANDP_PRNG_USE_ENC
#endif

#if !defined(RANDP_PRNG_USE_DAVIES_MEYER)
#define RANDP_PRNG_USE_DAVIES_MEYER DEFAULT_RANDP_PRNG_USE_DAVIES_MEYER
#endif

#if !defined(RANDP_POOL_SIZE_BYTES)
#define RANDP_POOL_SIZE_BYTES DEFAULT_RANDP_POOL_SIZE_BYTES
#endif

static_assert(RANDP_POOL_SIZE_BYTES > 0, "randp pool byte size must be positive");
static_assert((RANDP_POOL_SIZE_BYTES % 32) == 0, "randp pool byte size must be a multiple of 32");

#if !defined(RANDP_RESEED_INTERVAL)
#define RANDP_RESEED_INTERVAL DEFAULT_RANDP_RESEED_INTERVAL
#endif

static_assert(RANDP_RESEED_INTERVAL >= 1, "randp reseed interval must be positive");

#if defined(__x86_64__) && defined(__VAES__) && defined(__AVX2__)

#define RANDP_BLOCK_TYPE __m256i
#define RANDP_PRNG_TYPE aes_ctr_256_prng
#define RANDP_PRNG_RESEED aes_ctr_256_prng_reseed

#define RANDP_NEXT(PRNG)                                                  \
    (RANDP_PRNG_USE_ENC                                                   \
         ? (RANDP_PRNG_USE_DAVIES_MEYER                                   \
                ? aes_ctr_256_prng_enc_davies_meyer_next(PRNG)            \
                : aes_ctr_256_prng_enc_next(PRNG))                        \
         : (RANDP_PRNG_USE_DAVIES_MEYER                                   \
                ? aes_ctr_256_prng_dec_davies_meyer_next(PRNG)            \
                : aes_ctr_256_prng_dec_next(PRNG)))

#elif defined(__x86_64__) && defined(__AES__) && defined(__SSE4_1__)

#define RANDP_BLOCK_TYPE __m128i
#define RANDP_PRNG_TYPE aes_ctr_128_prng
#define RANDP_PRNG_RESEED aes_ctr_128_prng_reseed

#define RANDP_NEXT(PRNG)                                                  \
    (RANDP_PRNG_USE_ENC                                                   \
         ? (RANDP_PRNG_USE_DAVIES_MEYER                                   \
                ? aes_ctr_128_prng_enc_davies_meyer_next(PRNG)            \
                : aes_ctr_128_prng_enc_next(PRNG))                        \
         : (RANDP_PRNG_USE_DAVIES_MEYER                                   \
                ? aes_ctr_128_prng_dec_davies_meyer_next(PRNG)            \
                : aes_ctr_128_prng_dec_next(PRNG)))

#else
#error "Architecture not supported"
#endif

/// A pool of random bytes
struct randp
{
    RANDP_PRNG_TYPE prng;
    uint8_t pool[RANDP_POOL_SIZE_BYTES];
    int reseed_countdown;     ///< The PRNG is reseeded when this is 0.
    int rand_bytes_remaining; ///< The pool is regenerated when this is 0.
};

typedef struct randp randp;

static_assert(alignof(randp) == alignof(RANDP_BLOCK_TYPE),
              "randp must have the alignment of its block type");

static_assert(offsetof(randp, pool) % sizeof(RANDP_BLOCK_TYPE) == 0,
              "randp pool must start on a block boundary");

static_assert(sizeof(randp) <= PAGE_SIZE, "randp must fit in one page");

/// Regenerate the pool, reseeding the PRNG first if the reseed countdown has reached zero.
/**
* \param this_ the randp state
*/
static void
randp_regen(randp* this_)
{
    if (this_->reseed_countdown == 0)
    {
        RANDP_PRNG_RESEED(&this_->prng);
        this_->reseed_countdown = RANDP_RESEED_INTERVAL;
    }

    RANDP_BLOCK_TYPE* blocks = (RANDP_BLOCK_TYPE*)(&this_->pool[0]);

    constexpr int num_blocks = RANDP_POOL_SIZE_BYTES / sizeof(RANDP_BLOCK_TYPE);

    for (int i = 0; i < num_blocks; ++i)
    {
        blocks[i] = RANDP_NEXT(&this_->prng);
    }

    this_->rand_bytes_remaining = RANDP_POOL_SIZE_BYTES;
    --this_->reseed_countdown;
}

/// Deallocate a thread's \c randp pool when the thread exits.
/**
* \param p the randp pool, as passed to \c tss_set
*/
static void
randp_destroy(void* p)
{
    deallocate(p, sizeof(randp));
}

static tss_t randp_tss_key;
static once_flag randp_tss_once = ONCE_FLAG_INIT;

static void
randp_tss_key_init(void)
{
    if (tss_create(&randp_tss_key, randp_destroy) != thrd_success)
        errx(EXIT_FAILURE, "tss_create failed");
}

/// Public API
/**
* <code>void randp_bytes(void* buf, size_t n)</code>
* Fill a buffer with \a n random bytes
* This is similar to \c arc4random_buf.
*
* <code>uint8_t randp_u8();</code>
* <code>uint16_t randp_u16();</code>
* <code>uint32_t randp_u32();</code>
* <code>uint64_t randp_u64();</code>
* Return a uniform random unsigned integer
* This is similar to \c arc4random.
*
* <code>uint32_t randp_lt_u32(uint32_t upper_bound);</code>
* <code>uint64_t randp_lt_u64(uint64_t upper_bound);</code>
* Return a uniform random unsigned integer less than \a upper_bound
* This is similar to \c arc4random_uniform.
* If \a upper_bound is ≤ 1, \c 0 is returned.
*/

#if defined(RANDP_SINGLE_HEADER)
static
#endif
void
randp_bytes(void* buf, size_t n) [[gnu::nonnull]]
{
    static thread_local randp* this_ = nullptr;

    if (this_ == nullptr)
    {
#if defined(__cplusplus)
        this_ = (decltype(this_))allocate(sizeof(*this_));
#else
        this_ = (typeof(this_))allocate(sizeof(*this_));
#endif
        call_once(&randp_tss_once, randp_tss_key_init);
        if (tss_set(randp_tss_key, this_) != thrd_success)
            errx(EXIT_FAILURE, "tss_set failed");
    }

    uint8_t* dst = (uint8_t*)buf;

    while (n > 0)
    {
        if (this_->rand_bytes_remaining == 0)
            randp_regen(this_);

        uint8_t* src = &this_->pool[RANDP_POOL_SIZE_BYTES - this_->rand_bytes_remaining];

        const int m = (int)MIN(n, (size_t)this_->rand_bytes_remaining);

        (void)memcpy(dst, src, m);
        explicit_bzero(src, m);

        dst += m;
        this_->rand_bytes_remaining -= m;
        n -= m;
    }
}

[[nodiscard]]
#if defined(RANDP_SINGLE_HEADER)
static
#endif
uint8_t
randp_u8()
{
    uint8_t result = 0;
    randp_bytes(&result, sizeof(result));
    return result;
}

[[nodiscard]]
#if defined(RANDP_SINGLE_HEADER)
static
#endif
uint16_t
randp_u16()
{
    uint16_t result = 0;
    randp_bytes(&result, sizeof(result));
    return result;
}

[[nodiscard]]
#if defined(RANDP_SINGLE_HEADER)
static
#endif
uint32_t
randp_u32()
{
    uint32_t result = 0;
    randp_bytes(&result, sizeof(result));
    return result;
}

[[nodiscard]]
#if defined(RANDP_SINGLE_HEADER)
static
#endif
uint64_t
randp_u64()
{
    uint64_t result = 0;
    randp_bytes(&result, sizeof(result));
    return result;
}

/**
* \retval 0 If \a upper_bound ≤ 1.  This is similar to the behavior of
* \c arc4random_uniform.
*/
[[nodiscard]]
#if defined(RANDP_SINGLE_HEADER)
static
#endif
uint32_t
randp_lt_u32(uint32_t upper_bound)
{
    if (upper_bound <= 1)
        return 0;
    return random_bounded_nearlydivisionless32(upper_bound, randp_u32);
}

/**
* \retval 0 If \a upper_bound ≤ 1.  This is similar to the behavior of
* \c arc4random_uniform.
*/
[[nodiscard]]
#if defined(RANDP_SINGLE_HEADER)
static
#endif
uint64_t
randp_lt_u64(uint64_t upper_bound)
{
    if (upper_bound <= 1)
        return 0;
    return random_bounded_nearlydivisionless64(upper_bound, randp_u64);
}

#undef MIN
#undef RANDP_BLOCK_TYPE
#undef RANDP_PRNG_TYPE
#undef RANDP_PRNG_RESEED
#undef RANDP_NEXT

#if defined(__cplusplus)
}
#endif
