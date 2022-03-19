#include <stdlib.h>
#include <stdio.h>
#include "context.h"
#include "node.h"
#include "expression.h"

int get_address(Context *context)
{
    if (context->region == SECTION_DATA)
    {
        return context->address_data;
    }
    return context->address_text;
}

void set_address(Context *context, int value)
{
    if (context->region == SECTION_DATA)
    {
        context->address_data = value;
    }
    else
    {
        context->address_text = value;
    }
}

bool compare_expected(Context *context, struct Node *node)
{
    bool changed = true;
    ExpectedValue *current;

    bool node_is_null = true;
    int node_value = 0;
    if (node->expression)
    {
        node_is_null = !resolve_expression(context->symbol_map, node->expression, &node_value);
    }

    if (context->expected_values)
    {
        if (context->expected_value_head->next)
        {
            current = context->expected_value_head->next;
            if (current->node == node)
            {
                if (node->expression)
                {
                    if ((current->is_null && node_is_null) || (current->value == node_value))
                    {
                        changed = false;
                    }
                }
                else
                {
                    changed = false;
                }
            }
        }
        else
        {
            current = malloc(sizeof(ExpectedValue));
            context->expected_value_head->next = current;
            current->next = NULL;
        }
    }
    else
    {
        current = malloc(sizeof(ExpectedValue));
        context->expected_values = current;
        current->next = NULL;
    }

    current->node = node;
    if (node->expression)
    {
        current->is_null = node_is_null;
        current->value = node_value;
    }

    context->expected_value_head = current;
    return changed;
}

void free_expected_values(ExpectedValue *expected_values)
{
    ExpectedValue *current = expected_values;
    while (current)
    {
        ExpectedValue *next = current->next;
        free(current);
        current = next;
    }
}

