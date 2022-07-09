#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <stdbool.h>
#include "node.h"
#include "symbol.h"
#include "instruction.h"
#include "context.h"
#include "trace.h"

#define MAX_ITERATIONS 32

void assemble(FILE *file, Node *nodes, bool print_symbols);
bool assembler_pass(Context *context, Node *nodes);
bool assemble_node(Context *context, Node *node);
void assemble_opcode(Context *context, Trace *trace, char *name, int mode, Expression *expr);
void assemble_instruction(Context *context, Node *node);
void assemble_data(Context *context, Node *node);
void assemble_address(Context *context, Node *node);
void assemble_align(Context *context, Node *node);
void assemble_reserve(Context *context, Node *node);
bool resolve_node_symbol(Context *context, Node *node);
bool resolve_node_label(Context *context, Node *node);
void write_bytes(Context *context, Trace *trace, unsigned char *bytes, int size);
void pad_bytes(Context *context, Trace *trace, int size);

#endif

