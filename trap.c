#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include "trap.h"
#include "instruction.h"
#include "bits.h"
#include "control.h"


int trap(x16_t* machine, uint16_t instruction) {
    uint16_t vec = getbits(instruction, 0, 8);
    uint16_t* ptr;
    uint16_t c;
    int key;
    int base;

    switch (vec) {
    case TRAP_GETC:
        // TRAP GETC
        // read a single ASCII char and put it in R0
        // We do this by calling getchar, and setting the data to be
        // in the memory data register. It will get moved to R0 in the
        // WB stage.
        key = getchar();
        if (key == EOF) {
            perror("Getchar error");
            abort();
        }
        x16_set(machine, R_R0, (uint16_t) key);
        update_cond(machine, R_R0);
        break;

    case TRAP_OUT:
        // TRAP OUT
        // Write a single char in R0 to output
        c = x16_reg(machine, R_R0);
        putchar((char) c);
        fflush(stdout);
        break;

    case TRAP_PUTS:
        // TRAP PUTS
        // one char per word, with the word address stored in R0
        base = x16_reg(machine, R_R0);
        char c = (char) x16_memread(machine, base);
        while (c != '\0') {
            putc(c, stdout);
            c = (char) x16_memread(machine, ++base);
        }
        fflush(stdout);
        break;

    case TRAP_IN:
        // Read and echo a character, put it in R0
        printf("Enter a character: ");
        c = getchar();
        putc(c, stdout);
        fflush(stdout);
        // Setting the data to be in the memory data register.
        // It will get moved to R0 in the WB stage.
        x16_set(machine, R_R0, c);
        update_cond(machine, R_R0);
        break;

    case TRAP_PUTSP:
        // one char per byte (two bytes per word)
        //       here we need to swap back to
        //       big endian format */
        base = x16_reg(machine, R_R0);
        for (int val = x16_memread(machine, base);
            (val = x16_memread(machine, base)) != 0; base++) {
            char char1 = (val) & 0xff;
            putc(char1, stdout);
            fprintf(stderr, "Putting %c\n", char1);
            char char2 = (val) >> 8;
            if (char2) {
                putc(char2, stdout);
                fprintf(stderr, "Putting %c\n", char2);
            }
        }
        fflush(stdout);
        break;

    case TRAP_HALT:
        // TRAP HALT
        puts("HALT\n");
        fflush(stdout);
        return -1;

    default:
        // Bad trap vector
        abort();
    }

    return 0;
}
