#ifndef BITS_H_
#define BITS_H_

#include <stdint.h>
#include <stdbool.h>

// Get the nth bit in the number
uint16_t getbit(uint16_t number, int n);

// Get bits at location n that are the given number of bits wide
uint16_t getbits(uint16_t number, int n, int wide);

// Set the nth bit to the given bit value and return the number with the bit set
#ifdef setbit   // On some architectures, setbit is a macro
#undef setbit
#endif
uint16_t setbit(uint16_t number, int n);

// Clear the nth bit and return the number with the bit cleared
uint16_t clearbit(uint16_t number, int n);

// Sign extend a number with the given bit count to 16 bits
uint16_t sign_extend(uint16_t x, int bit_count);

// True if the number is positive
bool is_positive(uint16_t number);

// True if the number is negative
bool is_negative(uint16_t number);

#endif   // BITS_H_
