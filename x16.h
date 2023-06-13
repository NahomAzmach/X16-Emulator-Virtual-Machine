#ifndef X16_H_
#define X16_H_

#include <stdbool.h>
#include <stdint.h>

// Total amount of memory for 16 bit address
#define MAX_MEMORY                  65536

// Default code starting point
#define DEFAULT_CODESTART           0x3000

// 10 total registers, each of which is 16 bits. Most of them are general
// purpose, but a few have designated roles.
typedef enum {
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC,                   // program counter
    R_COND,                 // condition flag
} reg_t;

// There are 10 total registers
#define MAX_REGISTERS   10

// The X16 machine
typedef struct x16 x16_t;


// Initialize and return a new x16 machine. The program counter
// is set to the default start location DEFAULT_CODESTART
// All registers and memory are cleared to 0
x16_t* x16_create();

// Free all resources consumed by a machine
void x16_free(x16_t* machine);

// Get the program counter
uint16_t x16_pc(x16_t* machine);

// Get the condition register
uint16_t x16_cond(x16_t* machine);

// Get the contents of the machine register
uint16_t x16_reg(x16_t* machine, reg_t reg);

// Set the machine register
void x16_set(x16_t* machine, reg_t reg, uint16_t value);

// Read memory. Handles memory mapped registers
uint16_t x16_memread(x16_t* machine, uint16_t address);

// Memory write
void x16_memwrite(x16_t* machine, uint16_t address, uint16_t val);

// Get a pointer to the 16bit word in the given offset in memoty
uint16_t* x16_memory(x16_t* machine, uint16_t offset);

// Dump X16
void x16_print(x16_t* machine);

// Execute one single instruction. Return 0 on success or -1 for HALT
int x16_exec(x16_t* machine);

// This variable is set to 1 to turn on logging at each instruction execution
extern int LOG;

#endif   // X16_H_
