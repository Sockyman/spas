
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instruction.h"
#include "error.h"

/*char *mode_names[] =
{
    "implied",
    "immediate",
    "direct",
    "x_paged",
    "y_paged",
    "indexed",
};*/

char *mode_names[] =
{
    " ",
    "#",
    "@",
    "<x,@>",
    "<y,@>",
    "<x,y>",
};

extern Instruction instruction_set[];
extern int opcode_count;

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

bool instruction_opcode(char *name, int addressing, unsigned char *value)
{
    for (int i = 0; i < opcode_count; ++i)
    {
        if (instruction_set[i].addressing == addressing && !strcmp(instruction_set[i].name, name))
        {
            *value = i;
            return true;
        }
    }
    *value = 0;
    return false;
}

void print_instruction_set(void)
{
    for (int i = 0; i < opcode_count; ++i)
    {
        printf("%d %s\n", instruction_set[i].addressing, instruction_set[i].name);
    }
}

char *get_addressing_mode_name(int mode)
{
    return mode_names[mode];
}

