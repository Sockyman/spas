#ifndef NODE_H
#define NODE_H

#include "expression.h"
#include "trace.h"

enum node_type
{
    NODE_INSTRUCTION,
    NODE_LABEL,
    NODE_SYMBOL,
    NODE_DATA,
    NODE_ADDRESS,
};

enum data_mode
{
    DATA_BYTE,
    DATA_WORD,
};

typedef struct Datalist
{
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

Node *new_node(int type, char *name, int mode, Expression *expression, Datalist *datalist, Trace trace);

int node_size(Node *node);
void free_node(Node *node);
void print_node(Node *node);

Datalist *new_datalist(Expression *expression);
Datalist *append_datalist(Datalist *datalist, Expression *expression);
int datalist_size(int mode, Datalist *datalist);

#endif

