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

    Instruction *set = load_instruction_set();
    Context context = { NULL, 0, CONTEXT_INITIAL_CAPACITY, false, false, set, symbol_map };

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
        fwrite(context.output, context.address, 1, file);
    }

    free_symbol_map(context.symbol_map);
    free(context.output);
    free(set);
}

bool assembler_pass(Context *context, Node *nodes)
{
    context->address = 0;
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
            to_write[0] = inscuction_opcode(context->instruction_set, node->name, node->mode); 
            if (to_write[0] == 0xff)
            {
                print_error(&node->trace, "instruction %s:%d does not exist", node->name, node->mode);
            }
        }
    
        if (size != 1)
        {
            int value = 0;
            if (!resolve_expression(context->symbol_map, node->expression, &value))
            {
                // TODO: Error stuff
                print_error(&node->trace, "unable to resolve expression.");
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

    write_bytes(context, to_write, size);
}

void assemble_data(Context *context, Node *node)
{
    Datalist *current = node->datalist;
    while (current)
    {
        int value = 0;
        if (context->final_pass)
        {
            if (!resolve_expression(context->symbol_map, current->expression, &value))
            {
                // TODO: Error stuff
                print_error(&node->trace, "unable to resolve expression.");
            }
        }
        unsigned char to_write[] = { value, value >> 8 };
        int size = 1 + node->mode;
        write_bytes(context, to_write, size);

        current = current->next;
    }

}

void write_bytes(Context *context, unsigned char *bytes, int size)
{
    if (context->final_pass)
    {
        while (context->address + size >= context->capacity)
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
            // printf("Writing %02x: %02x\n", context->address + i, bytes[i]);
            context->output[context->address + i] = bytes[i];
        }
    }
    context->address += size;
}

void assemble_address(Context *context, Node *node)
{
    int value;
    resolve_expression(context->symbol_map, node->expression, &value);
    while (value > context->address)
    {
        unsigned char c = 0;
        write_bytes(context, &c, 1);
    }
}

