
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "expression.h"


Expression *generate_symbolic_expression(const char *symbol)
{
    Expression *expr = malloc(sizeof(Expression));
    expr->type = EXPR_SYMBOLIC;
    expr->value.symbol = strdup(symbol);
    return expr;
}

Expression *generate_integral_expression(int value)
{
    Expression *expr = malloc(sizeof(Expression));
    expr->type = EXPR_INTEGRAL;
    expr->value.integer = value;
    return expr;
}

Expression *generate_single_operand_expression(int operation, Expression *operand)
{
    Expression *expr = malloc(sizeof(Expression));
    expr->type = EXPR_SINGLE_OPERAND;
    expr->operation = operation;
    expr->value.single_operand = operand;
    return expr;
}

Expression *generate_dual_operand_expression(int operation, Expression *x, Expression *y)
{
    Expression *expr = malloc(sizeof(Expression));
    expr->type = EXPR_DUAL_OPERAND;
    expr->operation = operation;
    expr->value.dual_operand.x = x;
    expr->value.dual_operand.y = y;
    return expr;
}

void free_expression(Expression *expression)
{
    switch (expression->type)
    {
        case EXPR_INTEGRAL:
            break;
        case EXPR_SYMBOLIC:
            free(expression->value.symbol);
            break;
        case EXPR_SINGLE_OPERAND:
            free_expression(expression->value.single_operand);
            break;
        case EXPR_DUAL_OPERAND:
            free_expression(expression->value.dual_operand.x);
            free_expression(expression->value.dual_operand.y);
            break;
    }
    free(expression);
}


bool resolve_expression(Symbol *symbol_map, const Expression *expression, int *value)
{
    switch (expression->type)
    {
        case EXPR_INTEGRAL:
            *value = expression->value.integer;
            return true;
        case EXPR_SYMBOLIC:
            {
                int x;
                if (resolve_symbol(symbol_map, expression->value.symbol, &x))
                {
                    *value = x;
                    return true;
                }
                break;
            }
        case EXPR_SINGLE_OPERAND:
            {
                int x;
                if (resolve_expression(symbol_map, expression->value.single_operand, &x))
                {
                    *value = resolve_operation(expression->operation, x, 0);
                    return true;
                }
                break;
            }
        case EXPR_DUAL_OPERAND:
            {
                int x;
                int y;
                if (resolve_expression(symbol_map, expression->value.dual_operand.x, &x) && resolve_expression(symbol_map, expression->value.dual_operand.y, &y))
                {
                    *value = resolve_operation(expression->operation, x, y);
                    return true;
                }
                break;
            }
    }
    return false;
}

int resolve_operation(int operation, int x, int y)
{
    switch (operation)
    {
        case OPER_ADD:
            return x + y;
        case OPER_SUBTRACT:
            return x - y;
        case OPER_MULTIPLY:
            return x * y;
        case OPER_DIVIDE:
            return x / y;
        case OPER_NEGATE:
            return -x;
        case OPER_BIT_AND:
            return x & y;
        case OPER_BIT_OR:
            return x | y;
        case OPER_BIT_XOR:
            return x ^ y;
        case OPER_BIT_NOT:
            return ~x;
    }

    // TODO: Improve error checking.
    printf("Error: Unsupported operation.\n");
    return 0;
}

void print_expression(const Expression *expression)
{
    switch (expression->type)
    {
        case EXPR_SYMBOLIC:
            printf("%s", expression->value.symbol);
            break;
        case EXPR_INTEGRAL:
            printf("0x%02x", expression->value.integer);
            break;
        case EXPR_SINGLE_OPERAND:
            printf("(");
            printf("%s", operation_string(expression->operation));
            print_expression(expression->value.single_operand);
            printf(")");
            break;
        case EXPR_DUAL_OPERAND:
            printf("(");
            print_expression(expression->value.dual_operand.x);
            printf(" %s ", operation_string(expression->operation));
            print_expression(expression->value.dual_operand.y);
            printf(")");
            break;
    }
}

char *operation_string(int operation)
{
    switch (operation)
    {
        case OPER_ADD:
            return "+";
        case OPER_SUBTRACT:
            return "-";
        case OPER_MULTIPLY:
            return "*";
        case OPER_DIVIDE:
            return "/";
        case OPER_NEGATE:
            return "-";
        case OPER_BIT_AND:
            return "&";
        case OPER_BIT_OR:
            return "|";
        case OPER_BIT_XOR:
            return "^";
        case OPER_BIT_NOT:
            return "~";
    }
    return "???";
}

