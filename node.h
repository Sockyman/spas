#ifndef NODE_H
#define NODE_H

#include <stdbool.h>
#include <stdio.h>
#include "expression.h"
#include "trace.h"

enum node_type
{
    NODE_INSTRUCTION,
    NODE_LABEL,
    NODE_SYMBOL,
    NODE_DATA,
    NODE_ADDRESS,
    NODE_ALIGN,
    NODE_INCLUDE,
    NODE_SECTION,
    NODE_RESERVE,
};

enum data_mode
{
    DATA_BYTE,
    DATA_WORD,
};

enum include_mode
{
    INCLUDE_ALWAYS,
    INCLUDE_ONCE,
    INCLUDE_BIN,
};

enum section_mode
{
    SECTION_TEXT,
    SECTION_DATA,
};

typedef struct Datalist
{
    bool is_string;
    char *string;
    Expression *expression;
    struct Datalist *next;

} Datalist;

typedef struct Node
{
    char *name;
    int type;
    int mode;
    Expression *expression;
    Datalist *datalist;

    Trace trace;
    struct Node *next;
} Node;

Node *new_node(int type, const char *name, int mode, Expression *expression, Datalist *datalist, Trace trace);

void free_node(Node *node);
void print_node(Node *node);
void fprint_node(FILE *file, Node *node);

Node *new_instruction(char *name, int addressing_mode, Expression *operands);
Node *new_symbol(char *name, Expression *value);
Node *new_label(char *name);
Node *new_data(int mode, Datalist *datalist);
Node *new_address(Expression *expr);
Node *new_align(Expression *expr);
Node *new_include(const char *path, int mode);
Node *new_section(const char *section);
Node *new_reserve(Expression *expr);

Datalist *new_datalist(Expression *expression);
Datalist *new_data_string(char *string);
Datalist *append_datalist(Datalist *datalist, Datalist *value);
int datalist_size(int mode, Datalist *datalist);

void free_tree(Node *tree);
void print_tree(Node *tree);

Node *push_node(Node *start, Node *end);

void set_tree(Node *tree);
Node *get_tree(void);

#endif

