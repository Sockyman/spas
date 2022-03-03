#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdbool.h>
#include "context.h"

typedef struct Symbol
{
    char *key;
    int value;
    struct Symbol *next;
} Symbol;

bool define_symbol(Context *context, char *name, int value);
Symbol *get_symbol(Symbol *symbol_map, char *name);
bool resolve_symbol(Symbol *symbol_map, char *name, int *value);

void free_symbol(Symbol *symbol);
void print_symbol(Symbol *symbol);

void print_symbol_map(Symbol *symbol_map);
void free_symbol_map(Symbol *symbol_map);


#endif

