#ifndef SYNTAX_TREE_H
#define SYNTAX_TREE_H

#include "node.h"
#include "expression.h"

void initialize_node_tree();

void push_node(Node *node);
void push_instruction(char *name, int addressing_mode, Expression *operands);
void push_symbol(char *name, Expression *value);
void push_label(char *name);
void push_data(int mode, Datalist *datalist);
void push_address(Expression *expr);

Node *get_tree(void);
void free_tree(void);
void print_tree(void);

#endif

