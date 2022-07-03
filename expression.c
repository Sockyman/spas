
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "trace.h"
#include "error.h"
#include "expression.h"


Expression *symbolic_expression(Identifier *id)
{
    Expression *expr = malloc(sizeof(Expression));
    expr->type = EXPR_SYMBOLIC;
    if (id)
        expr->value.symbol = id;
    else
        expr->value.symbol = NULL;
    return expr;
}

Expression *integral_expression(int value)
{
    Expression *expr = malloc(sizeof(Expression));
    expr->type = EXPR_INTEGRAL;
    expr->value.integer = value;
    return expr;
}

Expression *char_expression(const char *string)
{
    unsigned char value = 0;
    int len = strlen(string);

    if (string[0] == '\\')
    {
        if (len == 2)
        {
            switch (string[1])
            {
                case '0':
                    value = 0;
                    break;
                case 'n':
                    value = '\n';
                    break;
                case '\\':
                    value = '\\';
                    break;
                case '\"':
                    value = '\"';
                    break;
                default:
                    print_error(global_trace, "invalid escape character \"\\%c\".", string[1]);
                    break;
            }
        }
        else
        {
            print_error(global_trace, "bad length %d.", len);
        }
    }
    else if (len == 1)
    {
        value = string[0];
    }
    else
    {
        print_error(global_trace, "invalid character length.");
    }

    return integral_expression(value);
}

Expression *single_operand_expression(int operation, Expression *operand)
{
    Expression *expr = malloc(sizeof(Expression));
    expr->type = EXPR_SINGLE_OPERAND;
    expr->operation = operation;
    expr->value.single_operand = operand;
    return expr;
}

Expression *dual_operand_expression(int operation, Expression *x, Expression *y)
{
    Expression *expr = malloc(sizeof(Expression));
    expr->type = EXPR_DUAL_OPERAND;
    expr->operation = operation;
    expr->value.dual_operand.x = x;
    expr->value.dual_operand.y = y;
    return expr;
}

Expression *ternary_expression(Expression *condition, Expression *a, Expression *b)
{
    Expression *expr = malloc(sizeof(Expression));
    expr->type = EXPR_TERNARY;
    expr->value.dual_operand.z = condition;
    expr->value.dual_operand.x = a;
    expr->value.dual_operand.y = b;
    return expr;
}

void free_expression(Expression *expression)
{
    switch (expression->type)
    {
        case EXPR_INTEGRAL:
            break;
        case EXPR_SYMBOLIC:
            free_identifier(expression->value.symbol);
            break;
        case EXPR_SINGLE_OPERAND:
            free_expression(expression->value.single_operand);
            break;
        case EXPR_TERNARY:
            free_expression(expression->value.dual_operand.z);
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
                if (resolve_expression(symbol_map, expression->value.dual_operand.x, &x) 
                        && resolve_expression(symbol_map, expression->value.dual_operand.y, &y))
                {
                    if ((y == 0) && (expression->operation == OPER_DIVIDE || expression->operation == OPER_MOD))
                    {
                        return false;
                    }
                    *value = resolve_operation(expression->operation, x, y);
                    return true;
                }
                break;
            }
        case EXPR_TERNARY:
            {
                int condition;
                int a;
                int b;
                if (resolve_expression(symbol_map, expression->value.dual_operand.z, &condition)
                        && resolve_expression(symbol_map, expression->value.dual_operand.x, &a)
                        && resolve_expression(symbol_map, expression->value.dual_operand.y, &b))
                {
                    *value = condition ? a : b;
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
            /*if (y == 0)
            {
                print_error(NULL, "divide by zero");
                return 0;
            }*/
            return x / y;
        case OPER_MOD:
            return x % y;
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
        case OPER_SHIFT_LEFT:
            return x << y;
        case OPER_SHIFT_RIGHT:
            return x >> y;
        case OPER_EQUAL:
            return x == y;
        case OPER_NOT_EQUAL:
            return x != y;
        case OPER_GREATER:
            return x > y;
        case OPER_GREATER_EQUAL:
            return x >= y;
        case OPER_LESS:
            return x < y;
        case OPER_LESS_EQUAL:
            return x <= y;
        case OPER_LOG_AND:
            return x && y;
        case OPER_LOG_OR:
            return x || y;
        case OPER_LOG_XOR:
            return !((x && y) || !(x || y));
        case OPER_LOG_NOT:
            return !x;
    }

    // TODO: Improve error checking.
    printf("Error: Unsupported operation.\n");
    return 0;
}

void bad_expression(Trace *trace, Expression *expression)
{
    print_error(trace, "unable to resolve expression.");
}

void print_expression(const Expression *expression)
{
    fprint_expression(stdout, expression);
}

void fprint_expression(FILE *file, const Expression *expression)
{
    char string[512];
    string[0] = 0;
    sprint_expression(string, expression);
    fprintf(file, "%s", string);
}

void sprint_expression(char *string, const Expression *expression)
{
    char s[256];
    switch (expression->type)
    {
        case EXPR_SYMBOLIC:
            //sprintf(s, "%s", expression->value.symbol);
            strcat(string, s);
            break;
        case EXPR_INTEGRAL:
            //printf("0x%02x", expression->value.integer);
            sprintf(s, "%d", expression->value.integer);
            strcat(string, s);
            break;
        case EXPR_SINGLE_OPERAND:
            sprintf(s, "(");
            strcat(string, s);
            sprintf(s, "%s", operation_string(expression->operation));
            strcat(string, s);
            sprint_expression(string, expression->value.single_operand);
            sprintf(s, ")");
            strcat(string, s);
            break;
        case EXPR_DUAL_OPERAND:
            sprintf(s, "(");
            strcat(string, s);
            sprint_expression(string, expression->value.dual_operand.x);
            sprintf(s, " %s ", operation_string(expression->operation));
            strcat(string, s);
            sprint_expression(string, expression->value.dual_operand.y);
            sprintf(s, ")");
            strcat(string, s);
            break;
        case EXPR_TERNARY:
            sprintf(s, "TERNARY");
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
        case OPER_MOD:
            return "%";
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
    return "UNKNOWN";
}

