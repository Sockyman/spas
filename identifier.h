#ifndef IDENTIFER_H
#define IDENTIFER_H

#include <stdio.h>
#include <stdbool.h>
#include "trace.h"

typedef struct Identifier
{
    char *name;
    struct Identifier *next;
} Identifier;

Identifier *identifier_add_scope(Identifier *id, char *str);
Identifier *get_identifier(char *str);
void free_identifier(Identifier *id);
void enter_scope(Identifier *id);
bool compare_identifier(Identifier *id_0, Identifier *id_1);
Identifier *clone_identifier(Identifier *to_clone);
void print_identifier(FILE *file, Identifier *id);

#endif

