#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdbool.h>
#include "context.h"
#include "identifier.h"

typedef struct Symbol
{
    Identifier *key;
    int value;
    struct Symbol *next;
} Symbol;

typedef struct StringList
{
    char *str;
    struct StringList *next;
} StringList;

bool define_symbol(Context *context, Identifier *name, int value);
Symbol *get_symbol(Symbol *symbol_map, Identifier *name);
bool resolve_symbol(Symbol *symbol_map, Identifier *identifier, int *value);

void free_symbol(Symbol *symbol);
void print_symbol(Symbol *symbol);

void print_symbol_map(Symbol *symbol_map);
void free_symbol_map(Symbol *symbol_map);


#endif

