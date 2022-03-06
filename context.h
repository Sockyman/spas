#ifndef CONTEXT_H
#define CONTEXT_H

#include "instruction.h"

struct Symbol;

#define CONTEXT_INITIAL_CAPACITY 64

typedef struct Context
{
    char *output;
    int address_text;
    int address_data;
    int capacity;
    bool is_changed;
    bool final_pass;
    int region;
    struct Symbol *symbol_map;
    
} Context;

int get_address(Context *context);
void set_address(Context *context, int value);

#endif

