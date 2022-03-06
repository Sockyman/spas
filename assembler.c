#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "node.h"
#include "symbol.h"
#include "assembler.h"
#include "instruction.h"
#include "error.h"
#include "resolve.h"


void assemble(FILE *file, Node *nodes)
{
    Symbol *symbol_map = malloc(sizeof(Symbol));
    symbol_map->key = strdup("NULL");
    symbol_map->value = 0;
    symbol_map->next = NULL;

    Context context = { NULL, 0, 0, CONTEXT_INITIAL_CAPACITY, false, false, SECTION_TEXT, symbol_map };

    if (error_count())
        return;

    context.output = malloc(sizeof(char) * CONTEXT_INITIAL_CAPACITY);

    for (int i = 0; i < 10; ++i)
    {
        assembler_pass(&context, nodes);
    }
    context.final_pass = true;
    assembler_pass(&context, nodes);

    if (!error_count())
    {
        fwrite(context.output, context.address_text, 1, file);
    }

    free_symbol_map(context.symbol_map);
    free(context.output);
}

bool assembler_pass(Context *context, Node *nodes)
{
    context->address_text = 0x0;
    context->address_data = 0x8000;
    bool changed = false;
    Node *current = nodes;
    while (current)
    {
        switch (current->type)
        {
            case NODE_LABEL:
                if (resolve_node_label(context, current))
                {
                    changed = true;
                }
                break;
            case NODE_SYMBOL:
                if (resolve_node_symbol(context, current))
                {
                    changed = true;
                }
                break;
            case NODE_INSTRUCTION:
                assemble_instruction(context, current);
                break;
            case NODE_DATA:
                assemble_data(context, current);
                break;
            case NODE_ADDRESS:
                assemble_address(context, current);
                break;
            case NODE_ALIGN:
                assemble_align(context, current);
                break;
            case NODE_SECTION:
                context->region = current->mode;
                break;
            case NODE_RESERVE:
                assemble_reserve(context, current);
                break;
            default:
                break;
        }
        
        current = current->next;
    }
    return changed;
}

void assemble_instruction(Context *context, Node *node)
{
    unsigned char to_write[3];
    int size = abs(instruction_size(node->mode));

    if (context->final_pass)
    {
        if (context->final_pass)
        {
            if (!instruction_opcode(node->name, node->mode, &to_write[0]))
            {
                print_error(&node->trace, "instruction \"%s%s\" does not exist", node->name, 
                    get_addressing_mode_name(node->mode));
            }
        }
    
        if (size != 1)
        {
            int value = 0;
            if (!resolve_expression(context->symbol_map, node->expression, &value))
            {
                bad_expression(&node->trace, node->expression);
            }
    
            if (size > 1)
            {
                to_write[1] = value;
            }
            if (size > 2)
            {
                to_write[2] = value >> 8;
            }
            if (size < -1)
            {
                to_write[1] = value >> 8;
            }
        }
    }

    write_bytes(context, &node->trace, to_write, size);
}

void assemble_data(Context *context, Node *node)
{
    Datalist *current = node->datalist;
    while (current)
    {
        if (current->is_string)
        {
            write_bytes(context, &node->trace, (unsigned char*)current->string, strlen(current->string));
        }
        else
        {
            int value = 0;
            if (context->final_pass)
            {
                if (!resolve_expression(context->symbol_map, current->expression, &value))
                {
                    bad_expression(&node->trace, current->expression);
                }
            }
            unsigned char to_write[] = { value, value >> 8 };
            int size = 1 + node->mode;
            write_bytes(context, &node->trace, to_write, size);
        }
        current = current->next;
    }
}

void assemble_address(Context *context, Node *node)
{
    int value;
    //TODO: error chekcing
    resolve_expression(context->symbol_map, node->expression, &value);
    while (value > get_address(context))
    {
        pad_bytes(context, &node->trace, 1);
    }
}

void assemble_align(Context *context, Node *node)
{
    int value;
    resolve_expression(context->symbol_map, node->expression, &value);
    while (get_address(context) % value != 0)
    {
        pad_bytes(context, &node->trace, 1);
    }

}

void assemble_reserve(Context *context, Node *node)
{
    int value;
    resolve_expression(context->symbol_map, node->expression, &value);
    pad_bytes(context, &node->trace, value);
}

void write_bytes(Context *context, Trace *trace, unsigned char *bytes, int size)
{
    if (context->final_pass)
    {
        if (context->region == SECTION_DATA)
        {
            print_error(trace, "cannot write to data region.");
        }
        else
        {
            while (context->address_text + size >= context->capacity)
            {
                context->capacity *= 2;
                char *new_output = realloc(context->output, context->capacity);
                if (!new_output)
                {
                    //TODO: Error
                    printf("Error: Unable to reallocate output array.\n");
                }
                context->output = new_output;
            }
            for (int i = 0; i < size; ++i)
            {
                context->output[context->address_text + i] = bytes[i];
            }
        }
    }
    set_address(context, get_address(context) + size);
}

void pad_bytes(Context *context, Trace *trace, int size)
{
    if (context->region == SECTION_TEXT)
    {
        for (int i = 0; i < size; ++i)
        {
            unsigned char c = 0;
            write_bytes(context, trace, &c, 1);
        }
    }
    else
    {
        set_address(context, get_address(context) + size);
    }

}
