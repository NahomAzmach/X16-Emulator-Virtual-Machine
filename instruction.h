#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include <stdint.h>
#include "bits.h"
#include "trap.h"

// An opcode is a command which tells the CPU to do some fundamental task
typedef enum {
    OP_BR = 0,          // branch
    OP_ADD,             // add
    OP_LD,              // load
    OP_ST,              // store
    OP_JSR,             // jump register
    OP_AND,             // bitwise and
    OP_LDR,             // load register
    OP_STR,             // store register
    OP_RTI,             // unused
    OP_NOT,             // bitwise not
    OP_LDI,             // load indirect
    OP_STI,             // store indirect
    OP_JMP,             // jump
    OP_RES,             // reserved (unused)
    OP_LEA,             // load effective address
    OP_TRAP             // execute trap
} opcode_t;

// The R_COND register stores condition flags which provide information
// about the most recently executed calculation.
// This is encoded in 3 bits, and only one of these bits must be set at the
// same time. These 3 bits are also set in the BR instruction.
typedef enum {
    FL_POS = 1,             // Positive
    FL_ZRO = 2,             // Zero
    FL_NEG = 4,             // Negative
} condition_t;

// ----------- Decoding instructions

// Get opcode from the instruction. The opcode is the highest 4 bits
// of the instruction.
opcode_t getopcode(uint16_t instruction);

// Get the immediate bit (5th bit)
uint16_t getimmediate(uint16_t instruction);

// Print an instruction in blocks of 4 bits. Useful for debugging
void print_instruction(uint16_t instruction);


// --------- Emitting instructions

// Emit an ADD instruction in register mode
uint16_t emit_add_reg(reg_t dst, reg_t src1, reg_t src2);

// Emit an ADD instruction in immediate mode
uint16_t emit_add_imm(reg_t dst, reg_t src1, uint16_t src2);

// Emit an AND instruction in register mode
uint16_t emit_and_reg(reg_t dst, reg_t src1, reg_t src2);

// Emit an AND instruction in immediate mode
uint16_t emit_and_imm(reg_t dst, reg_t src1, uint16_t src2);

// Emit a BR instruction
uint16_t emit_br(bool neg, bool zero, bool pos, int offset);

// Emit a JMP instruction
uint16_t emit_jmp(reg_t base);

// Emit a JSR instruction
uint16_t emit_jsr(uint16_t offset);

// Emit a JSRR instruction
uint16_t emit_jsrr(reg_t reg);

// Emit a LD instruction
uint16_t emit_ld(reg_t dst, uint16_t offset);

// Emit a LDI instruction
uint16_t emit_ldi(reg_t dst, uint16_t offset);

// Emit a LDR instruction
uint16_t emit_ldr(reg_t dst, reg_t base, uint16_t offset);

// Emit a LEA instruction
uint16_t emit_lea(reg_t dst, uint16_t offset);

// Emit a NOT instruction
uint16_t emit_not(reg_t dst, uint16_t src);

// Emit a ST instruction
uint16_t emit_st(reg_t src, uint16_t offset);

// Emit a STI instruction
uint16_t emit_sti(reg_t src, uint16_t offset);

// Emit a STR instruction
uint16_t emit_str(reg_t src, reg_t base, uint16_t offset);

// Emit a TRAP instruction
uint16_t emit_trap(trap_t vec);

// Emit a value
uint16_t emit_value(uint16_t val);

#endif  // INSTRUCTION_H_
