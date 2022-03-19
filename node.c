#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "node.h"
#include "error.h"
#include "expression.h"
#include "instruction.h"

Node *node_tree;

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


Node *new_instruction(char *name, int addressing_mode, Expression *operands)
{
    return new_node(NODE_INSTRUCTION, name, addressing_mode, operands, NULL, *global_trace);
}

Node *new_symbol(char *name, Expression *value)
{
    return new_node(NODE_SYMBOL, name, 0, value, NULL, *global_trace);
}

Node *new_label(char *name)
{
    return new_node(NODE_LABEL, name, 0, NULL, NULL, *global_trace);
}

Node *new_data(int mode, Datalist *datalist)
{
    return new_node(NODE_DATA, "", mode, NULL, datalist, *global_trace);
}

Node *new_address(Expression *expr)
{
    return new_node(NODE_ADDRESS, "", 0, expr, NULL, *global_trace);
}

Node *new_align(Expression *expr)
{
    return new_node(NODE_ALIGN, "", 0, expr, NULL, *global_trace);

}

Node *new_include(const char *path, int mode)
{
    return new_node(NODE_INCLUDE, path, mode, NULL, NULL, *global_trace);
}

Node *new_section(const char *section)
{
    int mode = SECTION_TEXT;
    if (!strcmp(section, "data"))
    {
        mode = SECTION_DATA;
    }
    else if (strcmp(section, "text"))
    {
        print_error(global_trace, "region does not exist: %s.", section);
    }
    return new_node(NODE_SECTION, "", mode, NULL, NULL, *global_trace);
}

Node *new_reserve(Expression *expr)
{
    return new_node(NODE_RESERVE, "", 0, expr, NULL, *global_trace);
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

void free_tree(Node *tree)
{
    Node *current = tree;
    while (current)
    {
        Node *next = current->next;
        free_node(current);
        current = next;
    }
}

void print_tree(Node *tree)
{
    Node *current = tree;
    while (current)
    {
        print_node(current);
        printf("\n");
        current = current->next;
    }
}

Node *push_node(Node *start, Node *end)
{
    if (start)
    {
        start->next = end;
        return start;
    }
    return end;
}

void set_tree(Node *tree)
{
    node_tree = tree;
}

Node *get_tree(void)
{

    return node_tree;
}

