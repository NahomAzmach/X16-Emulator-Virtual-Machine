#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "bits.h"

// Assert that the argument is a bit of either 0 or 1
#define assert_bit(a) if ((a) != 0 && (a) != 1) { assert(false); }

// Get the nth bit
uint16_t getbit(uint16_t number, int n) {
    assert_bit(n >= 0 && n <= 15);

    uint16_t bitMask = (1 << n);
    uint16_t bit = (number & bitMask) >> n;

    return bit;
}

// Get bits that are the given number of bits wide
uint16_t getbits(uint16_t number, int n, int wide) {
    assert_bit(n >= 0 && n <= 15);
    assert_bit(wide >= 0 && wide <= 15);

    // step 1 is to right shift he number by n
    number >>= n;

    // next we set the bits
    uint16_t mask = (1 << wide) - 1;

    // then we do AND
    uint16_t bits = mask & number;

    return bits;
}

// Set the nth bit to the given bit value and return the result
uint16_t setbit(uint16_t number, int n) {
    number = number | (1 << n);
}

// Clear the nth bit
uint16_t clearbit(uint16_t number, int n)
{
    number = number & ~(1 << n);
}

// Sign extend a number of the given bits to 16 bits
uint16_t sign_extend(uint16_t x, int bit_count) {
    if (getbit(x, bit_count - 1) == 1) {
        // making the mask if its a negative number
        uint16_t signExtension = (1 << (16 - bit_count)) - 1;


        uint16_t seNumber = x | (signExtension << bit_count);
        return seNumber;
    } else {
        // positive number
        return x;
    }
}

bool is_positive(uint16_t number) {
    return getbit(number, 15) == 0;
}

bool is_negative(uint16_t number) {
    return getbit(number, 15) == 1;
}

