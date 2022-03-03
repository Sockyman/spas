#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "node.h"
#include "expression.h"
#include "instruction.h"


int node_size(Node *node)
{
    switch (node->type)
    {
        case NODE_INSTRUCTION:
            return instruction_size(node->mode);
        case NODE_DATA:
            return datalist_size(node->mode, node->datalist);
        default:
            return 0;
    }
}

void print_node(Node *node)
{
    printf("%d: %s ", node->type, node->name);
    if (node->expression)
    {
        print_expression(node->expression);
    }
    printf(" at %s:%d", node->trace.filename, node->trace.line);
}


Node *new_node(int type, char *name, int mode, Expression *expression, Datalist *datalist, Trace trace)
{
    Node *node = malloc(sizeof(Node));
    node->type = type;
    node->name = strdup(name);
    node->mode = mode;
    node->expression = expression;
    node->datalist = datalist;
    node->trace = trace;
    node->trace.line -= 1;
    node->next = NULL;
    return node;
}

void free_node(Node *node)
{
    free(node->name);
    if (node->expression)
    {
        free_expression(node->expression);
    }

    Datalist *current = node->datalist;
    while (current)
    {
        Datalist *next = current->next;
        free_expression(current->expression);
        free(current);
        current = next;
    }

    free(node);
}

Datalist *new_datalist(Expression *expression)
{
    Datalist *datalist = malloc(sizeof(Datalist));
    datalist->expression = expression;
    datalist->next = NULL;
    return datalist;
}

Datalist *append_datalist(Datalist *datalist, Expression *expression)
{
    Datalist *current = datalist;
    while (current->next)
    {
        current = current->next;
    }
    current->next = new_datalist(expression);
    return datalist;
}

int datalist_size(int mode, Datalist *datalist)
{
    int size = 0;
    int element_size = 1 + mode;

    Datalist *current = datalist;
    while (current)
    {
        size += element_size;
        current = current->next;
    }

    return size;
}

