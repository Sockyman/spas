#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "node.h"
#include "symbol.h"
#include "assembler.h"
#include "instruction.h"
#include "error.h"

void assemble(FILE *file, Node *nodes, bool print_symbols)
{
    Symbol *symbol_map = malloc(sizeof(Symbol));
    symbol_map->key = get_identifier("NULL");
    symbol_map->value = 0;
    symbol_map->next = NULL;

    Context context = { NULL, 0, 0, CONTEXT_INITIAL_CAPACITY, false, false, 
        SECTION_TEXT, symbol_map, NULL, NULL };

    if (!error_count())
    {
        context.output = malloc(sizeof(char) * CONTEXT_INITIAL_CAPACITY);

        int iterations = 0;
        while (assembler_pass(&context, nodes) && iterations < MAX_ITERATIONS)
        {
            ++iterations;
        }
        //printf("iterations: %d\n", iterations);
        if (iterations >= MAX_ITERATIONS)
            print_error(NULL, 
                    "program cannot be resolved in maximum iterations.");

        if (!error_count())
        {
            context.final_pass = true;
            assembler_pass(&context, nodes);

            if (!error_count())
            {
                fwrite(context.output, context.address_text, 1, file);
            }
        }
    }
    
    if (print_symbols)
    {
        Symbol *current = context.symbol_map;
        while (current)
        {
            if (!strcmp(current->key->name, "NULL"))
            {
                current = current->next;
                continue;
            }
            Identifier *id = current->key;
            while (id)
            {
                fprintf(stderr, "%s", id->name);
                id = id->next;
                if (id)
                {
                    fprintf(stderr, ".");
                }
            }
            fprintf(stderr, "\t= %04x (%d)\n", current->value, current->value);
            current = current->next;
        }
        fprintf(stderr, "\n");
    }

    free_symbol_map(context.symbol_map);
    free_expected_values(context.expected_values);
    free(context.output);
}

bool assembler_pass(Context *context, Node *nodes)
{
    context->address_text = 0x0;
    context->address_data = 0x8000;
    context->region = SECTION_TEXT;
    Node *current = nodes;
    context->expected_value_head = context->expected_values;
    bool changed = false;

    while (current)
    {
        if (assemble_node(context, current))
            changed = true;
        current = current->next;
    }
    return changed;
}

bool assemble_node(Context *context, Node *node)
{
    switch (node->type)
    {
        case NODE_LABEL:
            resolve_node_label(context, node);
            break;
        case NODE_SYMBOL:
            resolve_node_symbol(context, node);
            break;
        case NODE_INSTRUCTION:
            assemble_instruction(context, node);
            break;
        case NODE_DATA:
            assemble_data(context, node);
            break;
        case NODE_ADDRESS:
            assemble_address(context, node);
            break;
        case NODE_ALIGN:
            assemble_align(context, node);
            break;
        case NODE_SECTION:
            context->region = node->mode;
            break;
        case NODE_RESERVE:
            assemble_reserve(context, node);
            break;
        default:
            break;
    }
    return compare_expected(context, node);
}

void assemble_opcode(Context *context, Trace *trace, char *name, int mode, Expression *expr)
{
    unsigned char to_write[3];
    int size = instruction_size(mode);

    if (context->final_pass)
    {
        if (context->final_pass)
        {
            if (!instruction_opcode(name, mode, &to_write[0]))
            {
                print_error(trace, "instruction \"%s%s\" does not exist", name, 
                    get_addressing_mode_name(mode));
            }
        }

        if (size != 1)
        {
            int value = 0;
            if (!resolve_expression(context->symbol_map, expr, &value))
            {
                bad_expression(trace, expr);
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
    write_bytes(context, trace, to_write, abs(size));
}

void assemble_instruction(Context *context, Node *node)
{
    if (!strcmp(node->name, "call") && node->mode == ADDR_DIRECT)
    {
        assemble_opcode(context, &node->trace, "phc", ADDR_IMPLIED, NULL);
        assemble_opcode(context, &node->trace, "jmp", ADDR_DIRECT, node->expression);
    }
    else if (!strcmp(node->name, "call") && node->mode == ADDR_INDEXED)
    {
        assemble_opcode(context, &node->trace, "phc", ADDR_IMPLIED, NULL);
        assemble_opcode(context, &node->trace, "jmp", ADDR_INDEXED, NULL);
        assemble_opcode(context, &node->trace, "nop", ADDR_IMPLIED, NULL);
        assemble_opcode(context, &node->trace, "nop", ADDR_IMPLIED, NULL);
    }
    else
        assemble_opcode(context, &node->trace, node->name, node->mode, node->expression);
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
    resolve_expression(context->symbol_map, node->expression, &value);

    //TODO: error chekcing
    if (value < get_address(context) && context->final_pass)
    {
        print_error(&node->trace, "Address already passed.");
    }

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

bool resolve_node_symbol(Context *context, Node *node)
{
    int value;
    if (resolve_expression(context->symbol_map, node->expression, &value))
    {
        int pvalue;
        if (!resolve_symbol(context->symbol_map, node->id, &pvalue) 
            || get_symbol(context->symbol_map, node->id)->value != value)
        {
            define_symbol(context, node->id, value);
            return true;

        }
    }
    return false;
}

bool resolve_node_label(Context *context, Node *node)
{
    int address = get_address(context);
    int value = 0;
    bool is_defined = resolve_symbol(context->symbol_map, node->id, &value);

    if (context->final_pass && is_defined && value != address)
    {
        print_error(&node->trace, 
                "Label \"%s\" at address %d is already defined", 
                node->name, address);
    }

    define_symbol(context, node->id, address);

    return value != address;
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
                    fprintf(stderr, 
                            "Error: Unable to reallocate output array.\n");
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

