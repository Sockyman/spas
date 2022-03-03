
%code requires
{
    #include "expression.h"
    #include "node.h"
    extern FILE *yyin;
}

%debug
%define parse.error verbose

%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"
#include "expression.h"
#include "node.h"
#include "syntax_tree.h"
#include "instruction.h"
#include "trace.h"
#include "error.h"

void yyerror(const char *s);
extern int yylex();

%}

%union
{
    int integer;
    char *string;
    Expression *expr;
    Datalist *datal;
}

%token IDENTIFIER STRING;
%token INTEGER CHARACTER
%token NEWLINE "\n"
%token ASSIGN "=" COLON ":" COMMA "," HASH "#"
%token PAREN_OPEN "(" PAREN_CLOSE ")" SQUARE_OPEN "[" SQUARE_CLOSE "]"
%token XREG "x" YREG "y"
%token BIT_AND "&" BIT_OR "|" BIT_NOT "~" BIT_XOR "^"
%token ADD "+" SUBTRACT "-" MULTIPLY "*" DIVIDE "/"
%token D_ADDR D_BYTE D_WORD D_INCLUDE D_INCLUDE_ONCE D_INCLUDE_BIN D_ALLIGN

%type <integer> INTEGER
%type <string> IDENTIFIER STRING CHARACTER
%type <expr> expression
%type <datal> data_list

%left BIT_OR
%left BIT_XOR
%left BIT_AND
%left ADD SUBTRACT
%left MULTIPLY DIVIDE
%left BIT_NOT
%left PAREN_OPEN


%%

program
    : statement_list
    | %empty
    ;

statement_list
    : statement_list statement
    | statement
    ;

statement
    : instruction
    | symbol
    | label
    | directive
    | "\n"
    ;

symbol
    : IDENTIFIER "=" expression "\n"     { push_symbol($1, $3); free($1); }
    ;

label
    : IDENTIFIER ":" statement          { push_label($1); free($1); }
    ;

directive
    : D_ADDR expression "\n"        { push_address($2); }
    | D_BYTE data_list "\n"         { push_data(DATA_BYTE, $2); }
    | D_WORD data_list "\n"         { push_data(DATA_WORD, $2); }
    | D_INCLUDE STRING "\n"
    | D_INCLUDE_ONCE STRING "\n"
    | D_INCLUDE_BIN STRING "\n"
    | D_ALLIGN expression "\n"
    ;

data_list
    : data_list "," expression      { $$ = append_datalist($1, $3); }
    | expression                    { $$ = new_datalist($1); }             
    ;

instruction
    : IDENTIFIER expression "\n"            { push_instruction($1, ADDR_DIRECT, $2); free($1); }
    | IDENTIFIER "#" expression "\n"            { push_instruction($1, ADDR_IMMEDIATE, $3); free($1); }
    | IDENTIFIER expression "," "x" "\n"    { push_instruction($1, ADDR_PAGED_X, $2); free($1); }
    | IDENTIFIER expression "," "y" "\n"    { push_instruction($1, ADDR_PAGED_Y, $2); free($1); }
    | IDENTIFIER "x" "," "y" "\n"           { push_instruction($1, ADDR_INDEXED, NULL); free($1); }
    | IDENTIFIER "\n"                               { push_instruction($1, ADDR_IMPLIED, NULL); free($1); }
    ;

expression
    : expression "+" expression                     { $$ = generate_dual_operand_expression(OPER_ADD, $1, $3); }
    | expression "-" expression   %prec ADD         { $$ = generate_dual_operand_expression(OPER_SUBTRACT, $1, $3); }   
    | "-" expression              %prec BIT_AND     { $$ = generate_single_operand_expression(OPER_NEGATE, $2); }
    | expression "*" expression                     { $$ = generate_dual_operand_expression(OPER_MULTIPLY, $1, $3); }
    | expression "/" expression                     { $$ = generate_dual_operand_expression(OPER_DIVIDE, $1, $3); }
    | "(" expression ")"                            { $$ = $2; }
    | expression "&" expression                     { $$ = generate_dual_operand_expression(OPER_BIT_AND, $1, $3); }
    | expression "|" expression                     { $$ = generate_dual_operand_expression(OPER_BIT_OR, $1, $3); }
    | expression "^" expression                     { $$ = generate_dual_operand_expression(OPER_BIT_XOR, $1, $3); }
    | "~" expression              %prec BIT_AND     { $$ = generate_single_operand_expression(OPER_BIT_NOT, $2); }
    | INTEGER                                       { $$ = generate_integral_expression($1); }
    | IDENTIFIER                                    { $$ = generate_symbolic_expression($1); free($1); }
    ;

%%


void yyerror(const char *s)
{
    print_error(&global_trace, s);
}

