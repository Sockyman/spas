#ifndef OPCODE_H
#define OPCODE_H

#define MAX_INSTRUCTIONS 256
#define MAX_INSTRUCTION_NAME 16

enum addressing_mode
{
    ADDR_IMPLIED,
    ADDR_IMMEDIATE,
    ADDR_DIRECT,
    ADDR_PAGED_X,
    ADDR_PAGED_Y,
    ADDR_INDEXED,
    ADDR_FUNCTION,
};

typedef struct Instruction
{
    char name[MAX_INSTRUCTION_NAME];
    int addressing;
} Instruction;

#endif

