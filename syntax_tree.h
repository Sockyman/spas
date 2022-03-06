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
void push_align(Expression *expr);
void push_include(const char *path, int mode);
void push_section(const char *section);
void push_reserve(Expression *expr);

Node *get_tree(void);
void free_tree(Node *tree);
void print_tree(Node *tree);

#endif

