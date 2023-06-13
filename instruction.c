#include <stdio.h>
#include "bits.h"
#include "instruction.h"


// Get opcode from the instruction. The opcode is the highest 4 bits
// of the instruction.
opcode_t getopcode(uint16_t instruction) {
    return (opcode_t) getbits(instruction, 12, 4);
}

// Get the immediate bit (5th bit)
uint16_t getimmediate(uint16_t instruction) {
    return getbit(instruction, 5);
}


// Note that u can change this function to print int values
// by changing the type and the sizeof
static void printbin(unsigned char value) {
    for (int i = 3; i >= 0; i--) {
        printf("%d", (value & (1 << i)) >> i);
    }
}

// Print an instruction in blocks of 4 bits. Useful for debugging
void print_instruction(uint16_t instruction) {
    for (int i = 0; i < 4; i++) {
        uint16_t block = (instruction >> (12 - 4*i)) & 0xff;
        printbin(block);
        if (i < 3) {
            printf(" ");
        }
    }
}


// Emit an ADD instruction in register mode
uint16_t emit_add_reg(reg_t dst, reg_t src1, reg_t src2) {
    return (OP_ADD << 12) | (dst << 9) | (src1 << 6) | (src2 & 0x1f);
}

// Emit an ADD instruction in immediate mode
uint16_t emit_add_imm(reg_t dst, reg_t src1, uint16_t src2) {
    return (OP_ADD << 12) | (dst << 9) | (src1 << 6) | (1 << 5) | (src2 & 0x1f);
}

// Emit an AND instruction in register mode
uint16_t emit_and_reg(reg_t dst, reg_t src1, reg_t src2) {
    return (OP_AND << 12) | (dst << 9) | (src1 << 6) | src2;
}

// Emit an ADD instruction in immediate mode
uint16_t emit_and_imm(reg_t dst, reg_t src1, uint16_t src2) {
    return (OP_AND << 12) | (dst << 9) | (src1 << 6) | (1 << 5) | (src2 & 0x1f);
}

// Emit a BR instruction
uint16_t emit_br(bool neg, bool zero, bool pos, int offset) {
    return (OP_BR << 12) | (neg << 11) | (zero << 10) | (pos << 9)
        | (offset & 0x1ff);
}

// Emit a JMP instruction
uint16_t emit_jmp(reg_t base) {
    return (OP_JMP << 12) | (base << 6);
}

// Emit a JSR instruction
uint16_t emit_jsr(uint16_t offset) {
    return (OP_JSR << 12) | (1 << 11) | (offset & 0x7ff);
}

// Emit a JSRR instruction
uint16_t emit_jsrr(reg_t reg) {
    return (OP_JSR << 12) | (reg << 6);
}

// Emit a LD instruction
uint16_t emit_ld(reg_t dst, uint16_t offset) {
    return (OP_LD << 12) | (dst << 9) | (offset & 0x1ff);
}

// Emit a LDI instruction
uint16_t emit_ldi(reg_t dst, uint16_t offset) {
    return (OP_LDI << 12) | (dst << 9) | (offset & 0x1ff);
}

// Emit a LDR instruction
uint16_t emit_ldr(reg_t dst, reg_t base, uint16_t offset) {
    return (OP_LDR << 12) | (dst << 9) | (base << 6) | (offset & 0x3f);
}

// Emit a LEA instruction
uint16_t emit_lea(reg_t dst, uint16_t offset) {
    return (OP_LEA << 12) | (dst << 9) | (offset & 0x1ff);
}

// Emit a NOT instruction
uint16_t emit_not(reg_t dst, uint16_t src) {
    return (OP_NOT << 12) | (dst << 9) | (src << 6) | 0x3f;
}

// Emit a ST instruction
uint16_t emit_st(reg_t src, uint16_t offset) {
    return (OP_ST << 12) | (src << 9) | (offset & 0x1ff);
}

// Emit a STI instruction
uint16_t emit_sti(reg_t src, uint16_t offset) {
    return (OP_STI << 12) | (src << 9) | (offset & 0x1ff);
}

// Emit a STR instruction
uint16_t emit_str(reg_t src, reg_t base, uint16_t offset) {
    return (OP_STR << 12) | (src << 9) | (base << 6) | (offset & 0x3f);
}

// Emit a TRAP instruction
uint16_t emit_trap(trap_t vec) {
    return (OP_TRAP << 12) | (vec & 0xff);
}

// Emit a value
uint16_t emit_value(uint16_t val) {
    return val;
}
