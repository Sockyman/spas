#ifndef INSTRUCTION_H
#define INSTRUCTION_H

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
};

typedef struct Instruction
{
    char name[MAX_INSTRUCTION_NAME];
    int addressing;
} Instruction;

int instruction_size(int addressing);
int inscuction_opcode(Instruction *set, char *name, int addressing);
Instruction *load_instruction_set(void);

void print_instruction_set(Instruction *set);

#endif

