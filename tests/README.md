# Tests

randp is fork-safe, just like arc4random.

randp passes PractRand tests through 512 GiB with these exhaustive options: <q>-tf 2</q>, <q>-te 1</q>.  And it scores a similar number of FIPS 140-2 successes and failures as arc4random and openssl-rand.

It has not been tested with [TestU01](https://en.wikipedia.org/wiki/TestU01) or [diehard](https://en.wikipedia.org/wiki/Diehard_tests).

## Dependencies

* [GCC 14](https://gcc.gnu.org/gcc-14/changes.html) or newer
  * [C23](https://en.cppreference.com/w/c/23) support was added in GCC 14.  randp uses the [`thread_local`](https://en.cppreference.com/w/c/keyword/thread_local) keyword.
  * clang and clang++ are not supported
* [Glibc 2.36](https://www.phoronix.com/news/GNU-C-Library-Glibc-2.36)
  * [arc4random](https://man7.org/linux/man-pages/man3/arc4random.3.html) functions were added in glibc 2.36, but the interface was <q>added as a basic loop wrapper around `getrandom()`</q>. [^arc4random_1] [^arc4random_2]
* `RNG_test` from [PractRand](https://github.com/planet36/PractRand)
* [Valgrind](https://valgrind.org/)
  * [KCachegrind](https://apps.kde.org/kcachegrind/) or QCachegrind to view the output
* `rngtest` from [rng-tools](https://github.com/nhorman/rng-tools)
* [openssl-rand](https://docs.openssl.org/master/man1/openssl-rand/)

[^arc4random_1]: https://lists.gnu.org/archive/html/info-gnu/2022-08/msg00000.html

[^arc4random_2]: https://lore.kernel.org/all/20220726195822.1223048-1-Jason@zx2c4.com/

## Example commands

* `make fork`
  * Verify randp is fork-safe.
* `make generate`
  * Generate some random bytes.
* `make single`
  * Verify the header-only version of randp compiles.
* `make profile-kgui` or `make profile-qgui`
  * Use a profiler on randp, and then run a visualizer.
* `make run-rngtest`
  * Test randomness using `rng-test`.
  * Takes about 5 minutes
* `make run-PractRand`
  * Test randomness using `RNG_test` (from PractRand).
  * Takes about 70 minutes!
  * To make this test more thorough (yet longer), change these options of `RNG_test`:
    * Change `-te` from `0` to `1`
    * Increase `-tlmax`
