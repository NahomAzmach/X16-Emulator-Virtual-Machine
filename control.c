#include <stdio.h>
#include <stdlib.h>
#include "bits.h"
#include "control.h"
#include "instruction.h"
#include "x16.h"
#include "trap.h"

// forward declarations
reg_t getsr1(uint16_t instruction);
reg_t getdr(uint16_t instruction);
reg_t getsr2(uint16_t instruction);
reg_t getsr(uint16_t instruction);

// Update condition code based on result
void update_cond(x16_t* machine, reg_t reg) {
    uint16_t result = x16_reg(machine, reg);
    if (result == 0) {
        x16_set(machine, R_COND, FL_ZRO);
    } else if (is_negative(result)) {
        x16_set(machine, R_COND, FL_NEG);
    } else {
        x16_set(machine, R_COND, FL_POS);
    }
}

// Execute a single instruction in the given X16 machine. Update
// memory and registers as required. PC is advanced as appropriate.
// Return 0 on success, or -1 if an error or HALT is encountered.
int execute_instruction(x16_t *machine)
{
    // Fetch the instruction and advance the program counter
    uint16_t pc = x16_pc(machine);
    uint16_t instruction = x16_memread(machine, pc);
    x16_set(machine, R_PC, pc + 1);

    // Decode the instruction and execute it
    uint16_t opcode = getopcode(instruction);
    switch (opcode)
    {
    case OP_ADD:
    {
        reg_t dst = getdr(instruction);
        reg_t src1 = getsr1(instruction);

        if (getimmediate(instruction) == 0)
        {
            reg_t src2 = getsr2(instruction);
            // doing the arithmetic
            x16_set(machine, dst, x16_reg(machine, src1)
            + x16_reg(machine, src2));
        } else {
            uint16_t imm = getbits(instruction, 0, 5);
            reg_t src2 = sign_extend(imm, 5);
            x16_set(machine, dst, x16_reg(machine, src1) + src2);
        }
        // set cc
        update_cond(machine, dst);
        break;
    }

    case OP_AND:
    {
        reg_t dr = getdr(instruction);
        reg_t sr1 = getsr1(instruction);
        if (getimmediate(instruction) == 0)
        {
            reg_t sr2 = getsr2(instruction);
            uint16_t result = x16_reg(machine, sr1) & x16_reg(machine, sr2);
            x16_set(machine, dr, result);
        } else {
            uint16_t imm5 = getbits(instruction, 0, 5);
            uint16_t sext_imm5 = sign_extend(imm5, 5);
            uint16_t result = x16_reg(machine, sr1) & sext_imm5;
            x16_set(machine, dr, result);
        }
        update_cond(machine, dr);
        break;
    }
    case OP_NOT: {
        reg_t dst = getdr(instruction);
        reg_t sr1 = getsr1(instruction);
        x16_set(machine, dst, ~x16_reg(machine, sr1));
        update_cond(machine, dst);
    }
    break;

    case OP_BR: {
        uint16_t n = getbit(instruction, 11);
        uint16_t z = getbit(instruction, 10);
        uint16_t p = getbit(instruction, 9);
        uint16_t pc_offset9 = getbits(instruction, 0, 9);
        int16_t offsetSext = sign_extend(pc_offset9, 9);

        bool branch = false;

        condition_t condition = x16_reg(machine, R_COND);
        bool branching = false;

        if (n && (condition & FL_NEG)) {
            branching = true;
        } else if (z && (condition & FL_ZRO)) {
            branching = true;
        } else if (p && (condition & FL_POS)) {
            branching = true;
        }
        if (branching) {
            x16_set(machine, R_PC, x16_reg(machine, R_PC)
            + offsetSext);
        }
                break;
    }
    case OP_JMP: {
        uint16_t baseR = getbits(instruction, 6, 8);
        // in the case RET is called
        if (baseR == R_R7)
        {
            x16_set(machine, R_PC, x16_reg(machine, R_R7));
        } else {
            x16_set(machine, R_PC, x16_reg(machine, baseR));
        }
    }
    break;
    case OP_JSR: {
        uint16_t bit11 = getbit(instruction, 11);
        x16_set(machine, R_R7, x16_reg(machine, R_PC));

        if (bit11 == 0)
        {
            uint16_t baseR = getbits(instruction, 6, 8);
            x16_set(machine, R_PC, x16_reg(machine, baseR));
        } else {
            uint16_t pcOffset11 = getbits(instruction, 0, 11);
            int16_t offsetSext = sign_extend(pcOffset11, 11);
            x16_set(machine, R_PC, x16_pc(machine) + offsetSext);
        }
    }
    break;
    case OP_LD: {
        reg_t dr = getdr(instruction);
        uint16_t pcOffset9 = getbits(instruction, 0, 9);
        int16_t offsetSext = sign_extend(pcOffset9, 9);
        uint16_t add = x16_reg(machine, R_PC) + offsetSext;
        uint16_t val = x16_memread(machine, add);

        x16_set(machine, dr, val);
        update_cond(machine, dr);
    }
    break;
    case OP_LDI:
    {
        reg_t dr = getdr(instruction);
        uint16_t pcOffset9 = getbits(instruction, 0, 9);
        int16_t offsetSext = sign_extend(pcOffset9, 9);
        uint16_t address = x16_reg(machine, R_PC) + offsetSext;
        uint16_t x = x16_memread(machine, address);
        uint16_t value = x16_memread(machine, x);

        x16_set(machine, dr, value);
        update_cond(machine, dr);
    }
    break;
    case OP_LDR:
    {
        reg_t dr = getdr(instruction);
        reg_t baseR = getbits(instruction, 6, 3);
        uint16_t offset6 = getbits(instruction, 0, 6);
        int16_t offsetSext = sign_extend(offset6, 6);
        uint16_t baseAddress = x16_reg(machine, baseR);
        uint16_t address = baseAddress + offsetSext;
        uint16_t value = x16_memread(machine, address);
        x16_set(machine, dr, value);
        update_cond(machine, dr);
    }
    break;
    case OP_LEA:
    {
        reg_t dr = getdr(instruction);
        uint16_t pcOffset9 = getbits(instruction, 0, 9);
        int16_t offsetSext = sign_extend(pcOffset9, 9);
        uint16_t address = x16_reg(machine, R_PC) + offsetSext;
        x16_set(machine, dr, address);
        update_cond(machine, dr);
    }
    break;
    case OP_ST:
    {
        reg_t sr = getsr(instruction);
        uint16_t pcOffset9 = getbits(instruction, 0, 9);
        int16_t offsetSext = sign_extend(pcOffset9, 9);
        uint16_t address = x16_reg(machine, R_PC) + offsetSext;
        x16_memwrite(machine, address, x16_reg(machine, sr));
    }
    break;
    case OP_STI:
    {
        reg_t sr = getsr(instruction);
        uint16_t pcOffset9 = getbits(instruction, 0, 9);
        int16_t offsetSext = sign_extend(pcOffset9, 9);
        uint16_t address = x16_reg(machine, R_PC) + offsetSext;
        uint16_t indirect = x16_memread(machine, address);
        uint16_t value = x16_reg(machine, sr);
        x16_memwrite(machine, indirect, value);
    }
    break;
    case OP_STR:
    {
        reg_t sr = getsr(instruction);
        reg_t baseR = getbits(instruction, 6, 3);
        uint16_t offset6 = getbits(instruction, 0, 6);
        int16_t offsetSext = sign_extend(offset6, 6);
        uint16_t baseAddress = x16_reg(machine, baseR);
        uint16_t address = baseAddress + offsetSext;
        uint16_t value = x16_reg(machine, sr);
        x16_memwrite(machine, address, value);
    }
    break;
    case OP_TRAP:
            // Execute the trap
            return trap(machine, instruction);
    case OP_RES:
    case OP_RTI:
    default:
            // Bad codes, never used
            abort();
    }
    return 0;
}

// get dr from the instruction
reg_t getdr(uint16_t instruction) {
    return (reg_t) getbits(instruction, 9, 3);
}

reg_t getsr(uint16_t instruction) {
    return (reg_t) getbits(instruction, 9, 3);
}

// Get sr1 from the instruction
reg_t getsr1(uint16_t instruction){
    return (reg_t) getbits(instruction, 6, 3);
}

// getting sr2 from the instruction
reg_t getsr2(uint16_t instruction) {
    return (reg_t) getbits(instruction, 0, 3);
}
