# Tests

randp is fork-safe, just like arc4random.

randp passes PractRand tests through 512 GiB with the options `-tf 2 -te 0`, in both the default build and a VAES build.

In 1,000,000 blocks of the FIPS 140-2 tests in `rngtest`, randp has 821 failures, arc4random has 814, and openssl-rand has 782.  mrand48 scores about the same with 774, so `rngtest` only rules out gross defects.

It has not been tested with [TestU01](https://en.wikipedia.org/wiki/TestU01) or [Diehard](https://en.wikipedia.org/wiki/Diehard_tests).

The `fork`, `generate`, `run-rngtest`, and `run-PractRand` targets also run mrand48 as a known-weak baseline.  It fails PractRand and is not fork-safe, so its output shows what a failure looks like.

## Dependencies

* Everything in [To build and use randp](../README.md#to-build-and-use-randp)
* [Glibc 2.36](https://www.phoronix.com/news/GNU-C-Library-Glibc-2.36) or newer
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
* `make -j 4 run-rngtest`
  * Test randomness using `rngtest`.
  * It takes about 1.5 minutes.
* `make run-PractRand`
  * Test randomness using `RNG_test` (from PractRand).
  * It takes about 54 minutes!
  * To make this test more thorough (yet longer), change these options of `RNG_test`:
    * Change `-te` from `0` to `1`.
    * Increase `-tlmax`.
