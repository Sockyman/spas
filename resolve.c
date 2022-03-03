#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "resolve.h"
#include "symbol.h"
#include "node.h"
#include "context.h"
#include "error.h"

bool resolve_node_symbol(Context *context, Node *node)
{
    int value;
    if (resolve_expression(context->symbol_map, node->expression, &value))
    {
        int pvalue;
        if (!resolve_symbol(context->symbol_map, node->name, &pvalue) 
            || get_symbol(context->symbol_map, node->name)->value != value)
        {
            define_symbol(context, node->name, value);
            return true;

        }
    }
    return false;
}

bool resolve_node_label(Context *context, Node *node)
{
    int value;
    bool is_defined = resolve_symbol(context->symbol_map, node->name, &value);

    if (context->final_pass && is_defined && value != context->address)
    {
        print_error(&node->trace, "Label \"%s\" at address %d is already defined", 
            node->name, context->address);
    }

    define_symbol(context, node->name, context->address);

    return value != context->address;
}

