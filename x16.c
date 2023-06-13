#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/select.h>
#include <string.h>
#include <stdlib.h>
#include "x16.h"
#include "instruction.h"

int LOG = 0;

// The X16 machine is defined local to the x16.c file.
// All other modules that want to manipulate X16 should do so
// using the functions defined in x16.h.
// Don't move this declaration to a .h file.
typedef struct x16 {
    // The memory of the computer is emulated by this array, each slot of
    // which stores a 16 bit value.
    uint16_t memory[MAX_MEMORY];

    // The register file contains R0-R7, PC and condition registers
    uint16_t registers[MAX_REGISTERS];
} x16_t;

// Special location in memory for memory mapped registers
typedef enum {
    MR_KBSR = 0xfe00,    // keyboard status
    MR_KBDR = 0xfe02     // keyboard data
} mmap_reg_t;



// Initialize the x16 machine
x16_t* x16_create() {
    x16_t* machine = (x16_t*) malloc(sizeof(x16_t));
    memset(machine, 0, sizeof(x16_t));
    x16_set(machine, R_PC, DEFAULT_CODESTART);         // default PC start
    return machine;
}

// Free the memory consumed by the machine
void x16_free(x16_t* machine) {
    free(machine);
}

// Get the program counter
uint16_t x16_pc(x16_t* machine) {
    return x16_reg(machine, R_PC);
}

// Get the condition register
uint16_t x16_cond(x16_t* machine) {
    return x16_reg(machine, R_COND);
}


// Get the register
uint16_t x16_reg(x16_t* machine, reg_t reg) {
    return machine->registers[reg];
}

// Set the machine register
void x16_set(x16_t* machine, reg_t reg, uint16_t value) {
    machine->registers[reg] = value;
}


// Check Key
static uint16_t check_key() {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

// Read memory. Handles memory mapped registers
uint16_t x16_memread(x16_t* machine, uint16_t address) {
    if (address == MR_KBSR) {
        // LOG = 0;
        if (check_key()) {
            machine->memory[MR_KBSR] = (1 << 15);
            machine->memory[MR_KBDR] = getchar();
            // printf("check_key: got %d\n", (int) machine->memory[MR_KBDR]);
            // LOG = 1;
        } else {
            machine->memory[MR_KBSR] = 0;
        }
    }
    return machine->memory[address];
}

// Memory write
void x16_memwrite(x16_t* machine, uint16_t address, uint16_t val) {
    machine->memory[address] = val;
}

// Get a pointer to the 16bit word in the given offset in memoty
uint16_t* x16_memory(x16_t* machine, uint16_t offset) {
    return &machine->memory[offset];
}

// Compute a hash value over memory. This gives a fingerprint of memory.
// If a byte changes in memory, the fingerprint should pick it up
static int compute_hash(unsigned char* data, int length) {
    const int p = 16777619;
    int hash = (int)2166136261;

    for (int i = 0; i < length; i++) {
        hash = (hash ^ data[i]) * p;
    }

    hash += hash << 13;
    hash ^= hash >> 7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;
    return hash;
}

// Dump X16 to stdout
void x16_print(x16_t* machine) {
    printf("Instruction: ");
    printf(", Memory: 0x%x\n",
        compute_hash((unsigned char *) machine->memory,
                     sizeof(uint16_t) * MAX_MEMORY));
    for (int i = 0; i < MAX_REGISTERS; i++) {
        printf("\tR%d(0x%x)\n", i, x16_reg(machine, (reg_t) i));
    }
}
