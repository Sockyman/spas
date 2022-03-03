
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instruction.h"
#include "error.h"

int instruction_size(int addressing)
{
    switch (addressing)
    {
        case ADDR_IMMEDIATE:
            return 2;
        case ADDR_DIRECT:
            return 3;
        case ADDR_IMPLIED:
        case ADDR_INDEXED:
            return 1;
        case ADDR_PAGED_X:
        case ADDR_PAGED_Y:
            return -2;
    }
    printf("Error: Unsuported addressing mode: %d", addressing);
    return 0;
}

int inscuction_opcode(Instruction *set, char *name, int addressing)
{
    for (int i = 0; i < MAX_INSTRUCTIONS; ++i)
    {
        if (set[i].addressing == addressing && !strcmp(set[i].name, name))
        {
            return i;
        }
    }
    return -1;
}

Instruction *load_instruction_set(void)
{
    Instruction *set = malloc(sizeof(Instruction) * MAX_INSTRUCTIONS);
    FILE *file = fopen("instruction_set.txt", "r");

    for (int i = 0; i < MAX_INSTRUCTIONS; ++i)
    {
        fscanf(file, "%d %s", &set[i].addressing, set[i].name);
    }
    fclose(file);
    return set;
}

void print_instruction_set(Instruction *set)
{
    for (int i = 0; i < MAX_INSTRUCTIONS; ++i)
    {
        printf("%d %s\n", set[i].addressing, set[i].name);
    }
}

