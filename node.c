#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "node.h"
#include "expression.h"
#include "instruction.h"

void print_node(Node *node)
{
    fprint_node(stdout, node);
}

void fprint_node(FILE *file, Node *node)
{
    switch (node->type)
    {
    case NODE_INSTRUCTION:
        fprintf(file, "  %s %s ", node->name, get_addressing_mode_name(node->mode));
        if (node->expression)
        {
            fprint_expression(file, node->expression);
        }
        break;
    case NODE_LABEL:
        fprintf(file, "%s:", node->name);
        break;
    case NODE_SYMBOL:
        fprintf(file, "%s = ", node->name);
        fprint_expression(file, node->expression);
        break;
    default:
        fprintf(file, "[%d]", node->type);
        if (strlen(node->name) > 0)
        {
            fprintf(file, " %s", node->name);
        }
        if (node->expression)
        {
            fprintf(file, " ");
            fprint_expression(file, node->expression);
        }
        break;
    }
}

Node *new_node(int type, const char *name, int mode, Expression *expression, Datalist *datalist, Trace trace)
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
        if (current->is_string)
        {
            free(current->string);
        }
        else
        {
            free_expression(current->expression);
        }
        free(current);
        current = next;
    }

    free(node);
}

Datalist *new_datalist(Expression *expression)
{
    Datalist *datalist = malloc(sizeof(Datalist));
    datalist->expression = expression;
    datalist->is_string = false;
    datalist->string = NULL;
    datalist->next = NULL;
    return datalist;
}

Datalist *new_data_string(char *string)
{
    Datalist *datalist = malloc(sizeof(Datalist));
    datalist->expression = NULL;
    datalist->is_string = true;
    datalist->string = strdup(string);
    datalist->next = NULL;
    return datalist;
}

Datalist *append_datalist(Datalist *datalist, Datalist *value)
{
    Datalist *current = datalist;
    while (current->next)
    {
        current = current->next;
    }
    current->next = value;
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

