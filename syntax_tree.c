
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "syntax_tree.h"
#include "node.h"
#include "error.h"
#include "trace.h"

Node *node_tree;

void initialize_node_tree()
{
    node_tree = NULL;
}

void push_node(Node *node)
{
    if (node_tree)
    {
        Node *current = node_tree;
        while (current->next)
        {
            current = current->next;
        }
        current->next = node;
    }
    else
    {
        node_tree = node;
    }
}

void push_instruction(char *name, int addressing_mode, Expression *operands)
{
    Node *node = new_node(NODE_INSTRUCTION, name, addressing_mode, operands, NULL, *global_trace);
    push_node(node);
}

void push_symbol(char *name, Expression *value)
{
    Node *node = new_node(NODE_SYMBOL, name, 0, value, NULL, *global_trace);
    push_node(node);
}

void push_label(char *name)
{
    Node *node = new_node(NODE_LABEL, name, 0, NULL, NULL, *global_trace);
    push_node(node);
}

void push_data(int mode, Datalist *datalist)
{
    Node *node = new_node(NODE_DATA, "", mode, NULL, datalist, *global_trace);
    push_node(node);
}

void push_address(Expression *expr)
{
    Node *node = new_node(NODE_ADDRESS, "", 0, expr, NULL, *global_trace);
    push_node(node);
}

void push_align(Expression *expr)
{
    Node *node = new_node(NODE_ALIGN, "", 0, expr, NULL, *global_trace);
    push_node(node);

}

void push_include(const char *path, int mode)
{
    Node *node = new_node(NODE_INCLUDE, path, mode, NULL, NULL, *global_trace);
    push_node(node);
}

void push_section(const char *section)
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
    Node *node = new_node(NODE_SECTION, "", mode, NULL, NULL, *global_trace);
    push_node(node);

}

void push_reserve(Expression *expr)
{
    Node *node = new_node(NODE_RESERVE, "", 0, expr, NULL, *global_trace);
    push_node(node);
}

Node *get_tree(void)
{
    return node_tree;
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

