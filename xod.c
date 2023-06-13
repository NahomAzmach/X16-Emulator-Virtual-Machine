#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <assert.h>
#include <string.h>
#include "instruction.h"

void usage() {
    fprintf(stderr, "Usage: ./xod file\n");
    exit(1);
}

char* decode(uint16_t instruction) {
    // The string is returned in this static buffer
    char* buf;
    char br[1024];
    br[0] = '\0';

    reg_t dst, src1, src2, base;
    uint16_t cond, vec;
    int16_t offset;
    int16_t value;

    opcode_t opcode = getopcode(instruction);
    switch (opcode) {
    case OP_ADD:
        dst = (reg_t) getbits(instruction, 9, 3);
        src1 = (reg_t) getbits(instruction, 6, 3);
        if (getimmediate(instruction) == 1) {
            value = sign_extend(getbits(instruction, 0, 5), 5);
            asprintf(&buf, "add    %%r%d, %%r%d, $%d",
                (int) dst, (int) src1, (int) value);
        } else {
            src2 = (reg_t) getbits(instruction, 0, 3);
            asprintf(&buf, "add    %%r%d, %%r%d, %%r%d",
                (int) dst, (int) src1, (int) src2);
        }
        break;

    case OP_AND:
        dst = (reg_t) getbits(instruction, 9, 3);
        src1 = (reg_t) getbits(instruction, 6, 3);
        if (getimmediate(instruction) == 1) {
            value = (uint16_t) sign_extend(getbits(instruction, 0, 5), 5);
            asprintf(&buf, "and    %%r%d, %%r%d, $%d",
                (int) dst, (int) src1, (int) value);
        } else {
            src2 = (reg_t) getbits(instruction, 0, 3);
            asprintf(&buf, "and    %%r%d, %%r%d, %%r%d",
                (int) dst, (int) src1, (int) src2);
        }
        break;

    case OP_NOT:
        dst = (reg_t) getbits(instruction, 9, 3);
        src1 = (reg_t) getbits(instruction, 6, 3);
        asprintf(&buf, "not    %%r%d, %%r%d",
            (int) dst, (int) src1);
        break;

    case OP_BR:
        offset = sign_extend(getbits(instruction, 0, 9), 9);
        cond = (uint16_t) getbits(instruction, 9, 3);
        snprintf(br, sizeof(br), "br");
        if (cond & FL_NEG) {
            strcat(br, "n");
        }
        if (cond & FL_ZRO) {
            strcat(br, "z");
        }
        if (cond & FL_POS) {
            strcat(br, "p");
        }
        asprintf(&buf, "%-6s $%d", br, offset);
        break;

    case OP_JMP:
        base = getbits(instruction, 6, 3);
        asprintf(&buf, "jmp    %%r%d", base);
        break;

    case OP_JSR:
        if (getbit(instruction, 11) == 1) {
            offset = sign_extend(getbits(instruction, 0, 11), 11);
            asprintf(&buf, "jsr    $%d", offset);
        } else {
            base = getbits(instruction, 6, 3);
            asprintf(&buf, "jsrr   %%r%d", base);
        }
        break;

    case OP_LD:
        dst = (reg_t) getbits(instruction, 9, 3);
        offset = sign_extend(getbits(instruction, 0, 9), 9);
        asprintf(&buf, "ld     %%r%d, $%d", dst, offset);
        break;

    case OP_LDI:
        dst = (reg_t) getbits(instruction, 9, 3);
        offset = sign_extend(getbits(instruction, 0, 9), 9);
        asprintf(&buf, "ldi    %%r%d, $%d", dst, offset);
        break;

    case OP_LDR:
        dst = (reg_t) getbits(instruction, 9, 3);
        base = getbits(instruction, 6, 3);
        offset = sign_extend(getbits(instruction, 0, 6), 6);
        asprintf(&buf, "ldr    %%r%d, %%r%d, $%d", dst, base,
            offset);
        break;

    case OP_LEA:
        dst = (reg_t) getbits(instruction, 9, 3);
        offset = sign_extend(getbits(instruction, 0, 9), 9);
        asprintf(&buf, "lea    %%r%d, $%d", dst, offset);
        break;

    case OP_ST:
        src1 = (reg_t) getbits(instruction, 9, 3);
        offset = sign_extend(getbits(instruction, 0, 9), 9);
        asprintf(&buf, "st     %%r%d, $%d", src1, offset);
        break;

    case OP_STI:
        src1 = (reg_t) getbits(instruction, 9, 3);
        offset = sign_extend(getbits(instruction, 0, 9), 9);
        asprintf(&buf, "sti    %%r%d, $%d", src1, offset);
        break;

    case OP_STR:
        src1 = (reg_t) getbits(instruction, 9, 3);
        base = (reg_t) getbits(instruction, 6, 3);
        offset = sign_extend(getbits(instruction, 0, 6), 6);
        asprintf(&buf, "str    %%r%d, %%r%d, $%d",
            src1, base, offset);
        break;

    case OP_TRAP:
        vec = getbits(instruction, 0, 8);
        if (vec == TRAP_GETC) {
            asprintf(&buf, "getc");
        } else if (vec == TRAP_OUT) {
            asprintf(&buf, "putc");
        } else if (vec == TRAP_PUTS) {
            asprintf(&buf, "puts");
        } else if (vec == TRAP_IN) {
            asprintf(&buf, "enter");
        } else if (vec == TRAP_PUTSP) {
            asprintf(&buf, "putsp");
        } else if (vec == TRAP_HALT) {
            asprintf(&buf, "halt");
        } else {
            asprintf(&buf, "-");
        }
        break;

    // case OP_RES:
    // case OP_RTI:
    default:
        // Consider everything else a value
        asprintf(&buf, "val    0x%x", (unsigned int) instruction);
        break;
    }

    return buf;
}

int main(int argc, char** argv) {
    if (argc > 2) {
        usage();
    }

    char* filename = "a.obj";
    if (argc == 2) {
        filename = argv[1];
    }

    FILE* fp = fopen(filename, "rb");
    uint16_t origin;
    if (fread(&origin, sizeof(origin), 1, fp) != 1) {
        fprintf(stderr, "Can't read origin\n");
        exit(2);
    }
    origin = ntohs(origin);

    printf("Origin: 0x%x\n", origin);
    int location = origin;
    uint16_t instruction;
    while (fread(&instruction, sizeof(instruction), 1, fp) == 1) {
        instruction = ntohs(instruction);
        printf("0x%x: ", location);
        print_instruction(instruction);
        char* str = decode(instruction);
        printf(" : %s\n", str);
        free(str);
        location++;
    }

    fclose(fp);
}
