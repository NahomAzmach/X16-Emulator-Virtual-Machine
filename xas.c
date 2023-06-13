#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include "instruction.h"
#include "x16.h"

#define MAX_LINE 256
#define MAX_LABEL_LENGTH 64
#define ORIGIN 0x3000
#define MAX_LABELS 100

typedef struct
{
    char label[MAX_LABEL_LENGTH];
    uint16_t address;
    int lineNumber;
} LabelEntry;

void usage()
{
    fprintf(stderr, "Usage: ./xas file");
    exit(1);
}

// HELPER FUNCTIONS
bool is_empty_or_comment(const char *line)
{
    while (isspace(*line))
    {
        // moves pointer to mext char
        line++;
    }

    // Skip lines that start with a comment symbol or contain only whitespace
    if (line[0] == '#' || line[0] == '\0'
    || strspn(line, " \t\n") == strlen(line)) {
        return true;
    }

    return false;
}

bool is_label_definition(const char *line)
{
    // Check if the line ends with a colon immediately after the label
    size_t length = strlen(line);
    bool isLabel = (length > 0 && line[length - 2] == ':');
    return (isLabel);
}

bool is_instruction(const char *line)
{
    // Check if the line starts with an instruction
    char opcode[10];
    if (sscanf(line, "%s", opcode) == 1)
    {
        if (strcmp(opcode, "add") == 0 || strcmp(opcode, "and") == 0 ||
            strcmp(opcode, "not") == 0 || strstr(opcode, "br") != NULL ||
            strcmp(opcode, "jmp") == 0 || strstr(opcode, "jsr") != NULL ||
            strcmp(opcode, "ld") == 0 || strcmp(opcode, "ldi") == 0 ||
            strcmp(opcode, "ldr") == 0 || strcmp(opcode, "lea") == 0 ||
            strcmp(opcode, "st") == 0 || strcmp(opcode, "sti") == 0 ||
            strcmp(opcode, "str") == 0 || strcmp(opcode, "trap") == 0 ||
            strcmp(opcode, "val") == 0 || strcmp(opcode, "getc") == 0 ||
            strcmp(opcode, "putc") == 0 || strcmp(opcode, "puts") == 0 ||
            strcmp(opcode, "enter") == 0 || strcmp(opcode, "putsp") == 0 ||
            strcmp(opcode, "halt") == 0)
        {
            return true;
        }
    }
    return false;
}

void parse_label(const char *line, LabelEntry *labels, int *numLabels,
 uint16_t *address, int lineNumber)
{
    char label[MAX_LABEL_LENGTH];  // for storing the labels
    // Copy the label name from the line, excluding the colon
    strncpy(label, line, sizeof(label) - 1);
    label[MAX_LABEL_LENGTH - 1] = '\0';

    // Remove leading and trailing whitespace from the label
    char *wsLabel = label;
    while (isspace(*wsLabel))
    {
        wsLabel++;
    }

    char *end = wsLabel + strlen(wsLabel) - 1;

    // recursively checks end of sting
    while (end > wsLabel && isspace(*end))
    {
        *end = '\0';
        end--;
    }

    int length = strlen(wsLabel) - 1;

    strncpy(labels[*numLabels].label, wsLabel,
    sizeof(labels[*numLabels].label) - 1);
    labels[*numLabels].label[length] = '\0';
    labels[*numLabels].address = *address;
    labels[*numLabels].lineNumber = lineNumber;
    (*numLabels)++;
}

reg_t parse_reg(int regstr) {
    int regNum = regstr;

    // Compare the integer value with the available register numbers
    if (regNum == 0) {
        return R_R0;
    } else if (regNum == 1) {
        return R_R1;
    } else if (regNum == 2) {
        return R_R2;
    } else if (regNum == 3) {
        return R_R3;
    } else if (regNum == 4) {
        return R_R4;
    } else if (regNum == 5) {
        return R_R5;
    } else if (regNum == 6) {
        return R_R6;
    } else if (regNum == 7) {
        return R_R7;
    } else {
        fprintf(stderr, "Invalid register: %d\n", regstr);
        exit(2);
    }
}

uint16_t compute_offset(char *label, LabelEntry *labels,
int lineCount, int numLabels)
{
    int address;
    bool f = false;
    for (int i = 0; i < numLabels; i++) {
        if (strcmp(label, labels[i].label) == 0) {
            address = labels[i].lineNumber;
            f = true;
        }
    }

    if (!f) {
        exit(2);
    }

    return (uint16_t)(address - lineCount);
}

// Parse instruction for assembler
int parse_instruction(const char *line, LabelEntry *labels,
                      int lineCount, int numLabels)
{
    // get the opcode from the line
    char opcode[10];
    sscanf(line, "%s", opcode);

    // parse operand
    char operands[3][60];

    /* skips past the opcode to only graB the operands
    // *s -- scanned string discarded
    // %hu three unsigned short integers to be present 
    in line after the skipped string.*/
    int numOperands = sscanf(line, "%*s %s %s %s", operands[0],
    operands[1], operands[2]);

    int param[3];

    // Check and parse operands with % or $
    for (int i = 0; i < numOperands; i++)
    {
        if (operands[i][0] == '%')
        {
            // Operand is a register
            param[i] = operands[i][2] - '0';
        } else if (operands[i][0] == '$') {
            // Operand is an immediate value
            param[i] = atoi(operands[i] + 1);
        } else {
            int off = compute_offset(operands[i], labels, lineCount, numLabels);
        }

        if (line != NULL)
        {
        line++;
        }
    }

    // Determine the opcode value
    short opcodeValue;
    if (strcmp(opcode, "add") == 0)
    {
        if (numOperands != 3)
        {
            fprintf(stderr, "Invalid number of operands_Add \n");
            return 2;
        }

        // gotta differenciate from imm and register
        // Check the format of the last operand
        if (operands[2][0] == '$')
        {
            // Operand is an immediate value
            return ntohs(emit_add_imm(param[0], param[1], param[2]));
        } else if (operands[2][0] == '%') {
            // Operand is a register
            return ntohs(emit_add_reg(param[0], param[1], param[2]));
        } else {
            fprintf(stderr, "Invalid format\n");
            return 2;
        }
    } else if (strcmp(opcode, "and") == 0) {
        if (numOperands != 3)
        {
            fprintf(stderr, "Invalid number of operands\n");
            return 2;
        }
        // gotta differenciate from imm and register
        // Check the format of the last operand
        if (operands[2][0] == '$')
        {
            // Operand is an immediate value
            return ntohs(emit_and_imm(param[0], param[1], param[2]));
        } else if (operands[2][0] == '%') {
            // Operand is a register
            return ntohs(emit_and_reg(param[0], param[1], param[2]));
        } else {
            fprintf(stderr, "Invalid format\n");
            return 2;
        }
    } else if (strcmp(opcode, "not") == 0) {
        reg_t dst = parse_reg(param[0]);
        reg_t src = parse_reg(param[1]);

        return ntohs(emit_not(dst, src));
    } else if (strcmp(opcode, "brn") == 0) {
        uint16_t offset = compute_offset(operands[0],
        labels, lineCount, numLabels);

        bool neg = true;
        bool zero = false;
        bool p = false;

        return ntohs(emit_br(neg, zero, p, offset));
    } else if (strcmp(opcode, "brp") == 0) {
        uint16_t offset = compute_offset(operands[0],
        labels, lineCount, numLabels);

        bool neg = false;
        bool zero = false;
        bool p = true;

        return ntohs(emit_br(neg, zero, p, offset));
    } else if (strcmp(opcode, "brz") == 0) {
        uint16_t offset = compute_offset(operands[0],
        labels, lineCount, numLabels);

        bool neg = false;
        bool zero = true;
        bool p = false;

        return ntohs(emit_br(neg, zero, p, offset));
    } else if (strcmp(opcode, "brzp") == 0) {
        uint16_t offset = compute_offset(operands[0],
        labels, lineCount, numLabels);

        bool neg = false;
        bool zero = true;
        bool p = true;

        return ntohs(emit_br(neg, zero, p, offset));
    } else if (strcmp(opcode, "brnp") == 0) {
        uint16_t offset = compute_offset(operands[0],
        labels, lineCount, numLabels);

        bool neg = true;
        bool zero = false;
        bool p = true;

        return ntohs(emit_br(neg, zero, p, offset));
    } else if (strcmp(opcode, "brnz") == 0) {
        uint16_t offset = compute_offset(operands[0],
        labels, lineCount, numLabels);

        bool neg = true;
        bool zero = true;
        bool p = false;

        return ntohs(emit_br(neg, zero, p, offset));
    } else if (strcmp(opcode, "brnzp") == 0) {
        uint16_t offset = compute_offset(operands[0],
        labels, lineCount, numLabels);

        bool neg = true;
        bool zero = true;
        bool p = true;

        return ntohs(emit_br(neg, zero, p, offset));
    } else if (strcmp(opcode, "br") == 0) {
        uint16_t offset = compute_offset(operands[0],
        labels, lineCount, numLabels);

        bool neg = false;
        bool zero = false;
        bool p = false;

        return ntohs(emit_br(neg, zero, p, offset));
    } else if (strcmp(opcode, "jmp") == 0) {
        reg_t baseR = parse_reg(param[0]);

        return (ntohs(emit_jmp(baseR)));
    } else if (strcmp(opcode, "jsr") == 0) {
        uint16_t offset = compute_offset(operands[0],
        labels, lineCount, numLabels);

        return ntohs(emit_jsr(offset));
    } else if (strcmp(opcode, "jsrr") == 0) {
        reg_t baseR = parse_reg(param[0]);

        return ntohs(emit_jsrr(baseR));
    } else if (strcmp(opcode, "ld") == 0) {
        reg_t dst = parse_reg(param[0]);
        uint16_t offset = compute_offset(operands[1],
        labels, lineCount, numLabels);

        return ntohs(emit_ld(dst, offset));
    } else if (strcmp(opcode, "ldi") == 0) {
        reg_t dst = parse_reg(param[0]);
        uint16_t offset = compute_offset(operands[1],
        labels, lineCount, numLabels);

        return ntohs(emit_ldi(dst, offset));
    } else if (strcmp(opcode, "ldr") == 0) {
        reg_t dst = parse_reg(param[0]);
        reg_t base = parse_reg(param[1]);
        uint16_t offset = (uint16_t) param[2];

        return ntohs(emit_ldr(dst, base, offset));
    } else if (strcmp(opcode, "lea") == 0) {
        reg_t dst = parse_reg(param[0]);
        uint16_t offset = compute_offset(operands[1],
        labels, lineCount, numLabels);

        return ntohs(emit_lea(dst, offset));
    } else if (strcmp(opcode, "st") == 0) {
        reg_t src = parse_reg(param[0]);
        uint16_t offset = compute_offset(operands[1],
        labels, lineCount, numLabels);

        return ntohs(emit_st(src, offset));
    } else if (strcmp(opcode, "sti") == 0) {
        reg_t src = parse_reg(param[0]);
        uint16_t offset = compute_offset(operands[1],
        labels, lineCount, numLabels);

        return ntohs(emit_sti(src, offset));
    } else if (strcmp(opcode, "str") == 0) {
        reg_t src = parse_reg(param[0]);
        reg_t base = parse_reg(param[1]);
        uint16_t offset = (uint16_t) param[2];

        return ntohs(emit_str(src, base, offset));
    } else if (strcmp(opcode, "val") == 0) {
        return ntohs(emit_value(param[0]));
    } else if (strcmp(opcode, "getc") == 0) {
        int result;
        result = emit_trap(TRAP_GETC);
        return ntohs(result);
    } else if (strcmp(opcode, "putc") == 0) {
        int result;
        result = emit_trap(TRAP_OUT);
        return ntohs(result);
    } else if (strcmp(opcode, "puts") == 0) {
        int result;
        result = emit_trap(TRAP_PUTS);
        return ntohs(result);
    } else if (strcmp(opcode, "enter") == 0) {
        int result;
        result = emit_trap(TRAP_IN);
        return ntohs(result);
    } else if (strcmp(opcode, "putsp") == 0) {
        int result;
        result = emit_trap(TRAP_PUTSP);
        return ntohs(result);
    } else if (strcmp(opcode, "halt") == 0) {
        int result;
        result = emit_trap(TRAP_HALT);
        return ntohs(result);
    } else {
        fprintf(stderr, "Invalid opcode: %s\n", opcode);
        return (2);  // opcode not valid
    }

    return 1;
}

void process_file(const char *inputFileName, FILE *outputFile)
{
    FILE *inputFile = fopen(inputFileName, "rb");
    if (!inputFile)
    {
        fprintf(stderr, "Cannot open input file: %s\n", inputFileName);
        fclose(outputFile);
        return;
    }

    uint16_t address = ORIGIN;
    LabelEntry labels[MAX_LABELS];
    int numLabels = 0;

    char line[MAX_LINE];
    int lineCount = 0;

    // First pass: Collect label definitions and their addresses
    while (fgets(line, sizeof(line), inputFile))
    {
        lineCount++;

        if (is_empty_or_comment(line))
        {
            lineCount--;
            continue;
        } else if (is_label_definition(line)) {
            lineCount--;
            parse_label(line, labels, &numLabels, &address, lineCount);
            address++;
        } else if (is_instruction(line)) {
            continue;
        } else {
            fprintf(stderr, "not a valid line %d: %s\n", lineCount, line);
            exit(2);
        }
    }

    fclose(inputFile);

    // open the input file again for reading during the second pass
    inputFile = fopen(inputFileName, "rb");
    if (!inputFile)
    {
        fprintf(stderr, "cant open input file: %s\n", inputFileName);
        fclose(outputFile);
        return;
    }

    // Second pass: Process instructions and pseudo-instructions
    address = ORIGIN;
    lineCount = 0;   // Reset line count

    while (fgets(line, sizeof(line), inputFile))
    {
        lineCount++;   // for valid instructions
        if (is_empty_or_comment(line) || is_label_definition(line))
        {
            lineCount--;
            continue;
        }
        if (is_instruction(line))
        {
            uint16_t instruction = parse_instruction(line,
            labels, lineCount, numLabels);
            fwrite(&instruction, sizeof(uint16_t), 1, outputFile);
            address++;
        }
    }
    fclose(inputFile);
    fclose(outputFile);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        usage();
    }

    const char *outputFile = "a.obj";

    // Open input file
    FILE *inputFile = fopen(argv[1], "rb");
    if (!inputFile)
    {
        fprintf(stderr, "cant open input file: %s\n", argv[1]);
        return 1;
    }

    // Create output file
    FILE *output = fopen(outputFile, "wb");
    if (!output)
    {
        fprintf(stderr, "cant create output file: %s\n", outputFile);
        fclose(inputFile);
        return 1;
    }

    uint16_t origin = htons(ORIGIN);
    fwrite(&origin, sizeof(uint16_t), 1, output);

    // process input file and write assembled instructions to the output
    process_file(argv[1], output);

    return 0;
}
