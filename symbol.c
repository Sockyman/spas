#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "symbol.h"
#include "identifier.h"


bool define_symbol(Context *context, Identifier *name, int value)
{
    Symbol *symbol = get_symbol(context->symbol_map, name);
    if (!symbol)
    {
        symbol = malloc(sizeof(Symbol));
        symbol->key = clone_identifier(name);
        symbol->next = NULL;
        symbol->value = value;

        if (context->symbol_map)
        {
            Symbol *current = context->symbol_map;
            while (current->next)
            {
                current = current->next;
            }
            current->next = symbol;
        }
        else
        {
            context->symbol_map = symbol;
        }
        return false;
    }
    symbol->value = value;
    return true;
}

Symbol *get_symbol(Symbol *symbol_map, Identifier *name)
{
    Symbol *current = symbol_map;
    while (current != NULL)
    {
        if (compare_identifier(current->key, name))
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

bool resolve_symbol(Symbol *symbol_map, Identifier *name, int *value)
{
    Symbol *symbol = get_symbol(symbol_map, name);
    if (symbol)
    {
        *value = symbol->value;
        return true;
    }
    return false;
}

void free_symbol(Symbol *symbol)
{
    free_identifier(symbol->key);
    free(symbol);
}

void free_symbol_map(Symbol *symbol_map)
{
    Symbol *current = symbol_map;

    while (current)
    {
        Symbol *next = current->next;
        free_symbol(current);
        current = next;
    }
}

void print_symbol(Symbol *symbol)
{
    fprintf(stderr, "{ %s: %d }", symbol->key->name, symbol->value);
}

void print_symbol_map(Symbol *symbol_map)
{
    printf("=== Symbols ===\n");
    Symbol *current = symbol_map;
    while (current)
    {
        print_symbol(current);
        printf("\n");
        current = current->next;
    }
}

