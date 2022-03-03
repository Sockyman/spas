#ifndef RESOLVE_H
#define RESOLVE_H

#include "symbol.h"
#include "node.h"
#include "context.h"

bool resolve_node_symbol(Context *context, Node *node);
bool resolve_node_label(Context *context, Node *node);

#endif

