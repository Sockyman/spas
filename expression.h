#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <stdbool.h>
#include "symbol.h"

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
Expression *generate_single_operand_expression(int operation, Expression *operand);
Expression *generate_dual_operand_expression(int operation, Expression *x, Expression *y);

void free_expression(Expression *expression);
bool resolve_expression(Symbol *symbol_map, const Expression *expression, int *value);
int resolve_operation(int operation, int x, int y);

void print_expression(const Expression *expression);
char *operation_string(int operation);

#endif

