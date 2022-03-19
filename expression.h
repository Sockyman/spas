#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <stdbool.h>
#include <stdio.h>
#include "symbol.h"
#include "trace.h"

enum expression_type
{
    EXPR_INTEGRAL,
    EXPR_SYMBOLIC,
    EXPR_SINGLE_OPERAND,
    EXPR_DUAL_OPERAND,
};

enum expression_operation
{
    OPER_ADD,
    OPER_SUBTRACT,
    OPER_MULTIPLY,
    OPER_DIVIDE,
    OPER_BIT_AND,
    OPER_BIT_OR,
    OPER_BIT_XOR,
    OPER_BIT_NOT,
    OPER_NEGATE,
    OPER_SHIFT_LEFT,
    OPER_SHIFT_RIGHT,
    OPER_EQUAL,
    OPER_NOT_EQUAL,
    OPER_GREATER,
    OPER_GREATER_EQUAL,
    OPER_LESS,
    OPER_LESS_EQUAL,
    OPER_LOG_AND,
    OPER_LOG_OR,
    OPER_LOG_XOR,
    OPER_LOG_NOT,
};

struct Expression;

struct dual_operands
{
    struct Expression *x;
    struct Expression *y;
};

union expression_value
{
    int integer;
    char *symbol;
    struct Expression *single_operand;
    struct dual_operands dual_operand;
};

typedef struct Expression
{
    int type;
    int operation;
    union expression_value value;

} Expression;


Expression *generate_symbolic_expression(const char *symbol);
Expression *generate_integral_expression(int value);
//Expression *generate_char_expression(const char *string);
Expression *generate_single_operand_expression(int operation, Expression *operand);
Expression *generate_dual_operand_expression(int operation, Expression *x, Expression *y);

void free_expression(Expression *expression);
bool resolve_expression(Symbol *symbol_map, const Expression *expression, int *value);
int resolve_operation(int operation, int x, int y);
void bad_expression(Trace *trace, Expression *expression);

void print_expression(const Expression *expression);
void fprint_expression(FILE *file, const Expression *expression);
void sprint_expression(char *string, const Expression *expression);
char *operation_string(int operation);

#endif

