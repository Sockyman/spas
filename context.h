#ifndef CONTEXT_H
#define CONTEXT_H

#include "instruction.h"

struct Symbol;
struct Node;

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
    struct ExpectedValue *expected_values;
    struct ExpectedValue *expected_value_head;
    
} Context;

typedef struct ExpectedValue
{
    struct Node *node;
    bool is_null;
    int value;
    struct ExpectedValue *next;
} ExpectedValue;

int get_address(Context *context);
void set_address(Context *context, int value);

bool compare_expected(Context *context, struct Node *node);
void free_expected_values(ExpectedValue *expected_values);

#endif

