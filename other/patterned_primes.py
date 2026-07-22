# SPDX-FileCopyrightText: Steven Ward
# SPDX-License-Identifier: MPL-2.0

"""
Usage/Example:

# Takes about 3.3 mins
time python3 patterned_primes.py > /tmp/patterned_primes.txt

# Takes about 32 secs
time pypy3 patterned_primes.py > /tmp/patterned_primes.PYPY.txt

# about 556K
ls -hop /tmp/patterned_primes*.txt
md5sum /tmp/patterned_primes*.txt

"""

import numpy as np
import sympy


def rotl(x: int, shift: int, width: int) -> int:
    """
    Rotate-left within `width` bits.

    Parameters
    ----------
    x : int
        Value to rotate.
    shift : int
        Rotation amount (can be any integer).
    width : int
        Bit-width to rotate within (e.g., 8, 16, 32, 64).

    Returns
    -------
    int
        Rotated value in the range [0, 2**width).
    """
    if width <= 0:
        raise ValueError("width must be positive")

    mask = (1 << width) - 1

    x &= mask

    # Normalize shift to be within [0, width)
    shift %= width

    if shift == 0:
        return x

    return ((x << shift) | (x >> (width - shift))) & mask


_FORBIDDEN = ('000', '111', '00100', '11011',
              #'001100',
              #'110011',
              #'1100',
              #'0011',
              )

def has_forbidden_bit_sequence(s: str) -> bool:
    '''Determine if s has a forbidden bit sequence.'''

    return any(pat in s for pat in _FORBIDDEN)


# pylint: disable=too-many-locals,too-many-branches
def main() -> None:
    '''Search for patterned primes and print a linspace selection of the 32-bit ones.'''

    primes_32 = []

    for bits in (8, 16, 32):

        print(f'# {bits=}')
        bits_half = bits // 2

        #bits_mask = (2**bits) - 1
        #bits_half_mask = bits_mask >> bits_half

        #start = ((2**bits) >> 2) | 1 # odd
        start = (0b01 << (bits-2)) | 1 # odd

        #end = 2**bits - (start - 1)
        end = 0b11 << (bits-2)

        print(f'# start = {start}  {hex(start)}  0b{start:0{bits}b}')
        print(f'# end   = {end}  {hex(end)}  0b{end:0{bits}b} (exclusive)')

        num_bytes = bits // 8

        num_found_primes = 0

        #for i in range(0, 2**bits):
        #for i in range(2**(bits-1), 2**bits):
        #for i in range(2**(bits-1), 2**bits - (2**bits >> 2)):
        for i in range(start, end, 2):

            if i.bit_count() != bits_half:
                continue

            if not sympy.isprime(i):
                continue

            skip = False

            # Rotate the value to find forbidden bit patterns.
            for shift in range(0, 8):
                i2 = rotl(i, shift, bits)
                bit_count_per_byte = tuple(
                        b.bit_count() for b in i2.to_bytes(num_bytes, byteorder='big'))

                # Detect too few or too many set bits in each byte.
                if any(n < 4-1 or n > 4+1 for n in bit_count_per_byte):
                    skip = True
                    break

                if num_bytes > 2:
                    # At least one of the bytes must have popcount = 4.
                    if all(n != 4 for n in bit_count_per_byte):
                        skip = True
                        break

                s = f'{i2:0{bits}b}'

                if has_forbidden_bit_sequence(s):
                    skip = True
                    break

            if skip:
                continue

            s = f'{i:0{bits}b}'

            if bits == 32:
                primes_32.append(i)

            print(f'{i=}  {hex(i)=}  0b{s}')
            num_found_primes += 1

        print(f'# {num_found_primes=}')
        print()

    bits = 32
    if not primes_32:
        print("# no 32-bit primes found; skipping selection")
        return

    for num in (4, 8, 16, 32):

        print(f'# select {num} using numpy linspace')

        idx = np.round(
                np.linspace(start=0, stop=len(primes_32)-1, num=num, endpoint=True)).astype(int)
        selection = np.array(primes_32)[idx]

        for val in selection:
            i = int(val)
            s = f'{i:0{bits}b}'
            print(f'{i=}  {hex(i)=}  0b{s}')
        print()


if __name__ == "__main__":
    main()
