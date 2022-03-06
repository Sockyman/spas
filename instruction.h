#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdbool.h>
#include "opcode.h"

int instruction_size(int addressing);
bool instruction_opcode(char *name, int addressing, unsigned char *value);

void print_instruction_set(void);
char *get_addressing_mode_name(int mode);

#endif

