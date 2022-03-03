#ifndef CONTEXT_H
#define CONTEXT_H

#include "instruction.h"

struct Symbol;

#define CONTEXT_INITIAL_CAPACITY 64

typedef struct Context
{
    char *output;
    int address;
    int capacity;
    bool is_changed;
    bool final_pass;
    Instruction *instruction_set;
    struct Symbol *symbol_map;
    
} Context;

#endif

