#ifndef TRAP_H_
#define TRAP_H_

#include "x16.h"

/* TRAP Codes */
typedef enum {
    TRAP_GETC = 0x20,   // get character from keyboard, not echoe
    TRAP_OUT = 0x21,    // output a character
    TRAP_PUTS = 0x22,   // output a word string
    TRAP_IN = 0x23,     // get character from keyboard, echoed
    TRAP_PUTSP = 0x24,  // output a byte string
    TRAP_HALT = 0x25    // halt the program
} trap_t;

// Service the trap instruction. Return -1 to halt or 0 to continue
int trap(x16_t* machine, uint16_t instruction);

#endif  // TRAP_H_
