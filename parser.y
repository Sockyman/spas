%code requires
{
    #include "expression.h"
    #include "node.h"
    #include "identifier.h"
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
#include "instruction.h"
#include "trace.h"
#include "error.h"
#include "identifier.h"

void yyerror(const char *s);
extern int yylex();

%}

%union
{
    int integer;
    char *string;
    Expression *expr;
    Datalist *datal;
    Node *node;
    Identifier *id;
}

%token IDENTIFIER STRING
%token INTEGER
%token NEWLINE "\n" DOT "."
%token ASSIGN "=" COLON ":" COMMA "," HASH "#"
%token PAREN_OPEN "(" PAREN_CLOSE ")" SQUARE_OPEN "[" SQUARE_CLOSE "]"
%token XREG "x" YREG "y"
%token BIT_AND "&" BIT_OR "|" BIT_NOT "~" BIT_XOR "^"
%token ADD "+" SUBTRACT "-" MULTIPLY "*" DIVIDE "/" MOD "%"
%token SHIFT_L "<<" SHIFT_R ">>"
%token EQUAL "==" NOT_EQUAL "!=" GREATER ">" GREATER_EQUAL ">=" LESS "<" LESS_EQUAL "<="
%token LOG_AND "&&" LOG_OR "||" LOG_NOT "!" LOG_XOR "^^"
%token D_ADDR D_BYTE D_WORD D_INCLUDE D_INCLUDE_ONCE D_INCLUDE_BIN D_ALIGN
%token D_SECTION D_RESERVE
%token D_MACRO D_ENDMACRO
%token QMARK


%type <integer> INTEGER
%type <string> IDENTIFIER STRING
%type <expr> expression
%type <datal> data_list data_entry
%type <node> statement statement_list instruction directive symbol label
%type <id> ident


%right QMARK COLON
%left LOG_OR
%left LOG_XOR
%left LOG_AND
%left BIT_OR
%left BIT_XOR
%left BIT_AND
%left EQUAL NOT_EQUAL
%left GREATER GREATER_EQUAL LESS LESS_EQUAL
%left SHIFT_L SHIFT_R
%left ADD SUBTRACT
%left MULTIPLY DIVIDE MOD
%left BIT_NOT LOG_NOT
%left PAREN_OPEN

%%

program
    : statement_list    { set_tree($1); }
    | %empty
    ;

statement_list
    : statement statement_list  { $$ = push_node($1, $2); }
    | statement     { $$ = $1; }
    ;

statement
    : instruction
    | symbol
    | label
    | directive
    | error "\n"    { $$ = NULL; }
    | "\n"          { $$ = NULL; }
    ;

symbol
    : ident "=" expression "\n"    { $$ = new_symbol($1, $3); }
    ;

label
    : ident ":" statement      { $$ = new_label($1); enter_scope($1); }
    ;

directive
    : D_ADDR expression "\n"        { $$ = new_address($2); }
    | D_BYTE data_list "\n"         { $$ = new_data(DATA_BYTE, $2); }
    | D_WORD data_list "\n"         { $$ = new_data(DATA_WORD, $2); }
    | D_INCLUDE STRING "\n"         { $$ = new_include($2, INCLUDE_ALWAYS); free($2); }
    | D_INCLUDE_ONCE STRING "\n"    { $$ = new_include($2, INCLUDE_ONCE); free($2); }
    | D_INCLUDE_BIN STRING "\n"     { $$ = new_include($2, INCLUDE_BIN); }
    | D_ALIGN expression "\n"       { $$ = new_align($2); }
    | D_SECTION IDENTIFIER "\n"     { $$ = new_section($2); free($2); }
    | D_RESERVE expression "\n"     { $$ = new_reserve($2); }
    ;

/*macro
     : D_MACRO IDENTIFIER "\n" statement_list D_ENDMACRO "\n"
     | D_MACRO IDENTIFIER "#" "\n" statement_list D_ENDMACRO "\n"
     | D_MACRO IDENTIFIER "x" "," "y" "\n" statement_list D_ENDMACRO "\n"
     | D_MACRO IDENTIFIER "(" ")" "\n" statement_list D_ENDMACRO "\n"
     ;*/

data_list
    : data_list "," data_entry      { $$ = append_datalist($1, $3); }
    | data_entry                    { $$ = $1; }
    ;

data_entry
    : expression                    { $$ = new_datalist($1); }
    | STRING                        { $$ = new_data_string($1); free($1); }
    ;

instruction
    : IDENTIFIER expression "\n"            { $$ = new_instruction($1, ADDR_DIRECT, $2); free($1); }
    | IDENTIFIER "#" expression "\n"        { $$ = new_instruction($1, ADDR_IMMEDIATE, $3); free($1); }
    | IDENTIFIER expression "," "x" "\n"    { $$ = new_instruction($1, ADDR_PAGED_X, $2); free($1); }
    | IDENTIFIER expression "," "y" "\n"    { $$ = new_instruction($1, ADDR_PAGED_Y, $2); free($1); }
    | IDENTIFIER "x" "," "y" "\n"           { $$ = new_instruction($1, ADDR_INDEXED, NULL); free($1); }
    | IDENTIFIER "\n"                       { $$ = new_instruction($1, ADDR_IMPLIED, NULL); free($1); }
    ;

expression
    : expression "+" expression         { $$ = dual_operand_expression(OPER_ADD, $1, $3); }
    | expression "-" expression         { $$ = dual_operand_expression(OPER_SUBTRACT, $1, $3); }
    | "-" expression    %prec BIT_AND   { $$ = single_operand_expression(OPER_NEGATE, $2); }
    | "+" expression    %prec BIT_AND   { $$ = $2; }
    | expression "*" expression         { $$ = dual_operand_expression(OPER_MULTIPLY, $1, $3); }
    | expression "/" expression         { $$ = dual_operand_expression(OPER_DIVIDE, $1, $3); }
    | expression "%" expression         { $$ = dual_operand_expression(OPER_MOD, $1, $3); }
    | "(" expression ")"                { $$ = $2; }
    | expression "&" expression         { $$ = dual_operand_expression(OPER_BIT_AND, $1, $3); }
    | expression "|" expression         { $$ = dual_operand_expression(OPER_BIT_OR, $1, $3); }
    | expression "^" expression         { $$ = dual_operand_expression(OPER_BIT_XOR, $1, $3); }
    | "~" expression    %prec BIT_AND   { $$ = single_operand_expression(OPER_BIT_NOT, $2); }
    | "!" expression                    { $$ = single_operand_expression(OPER_LOG_NOT, $2); }
    | expression "<<" expression        { $$ = dual_operand_expression(OPER_SHIFT_LEFT, $1, $3); }
    | expression ">>" expression        { $$ = dual_operand_expression(OPER_SHIFT_RIGHT, $1, $3); }
    | expression "==" expression        { $$ = dual_operand_expression(OPER_EQUAL, $1, $3); }
    | expression "!=" expression        { $$ = dual_operand_expression(OPER_NOT_EQUAL, $1, $3); }
    | expression "&&" expression        { $$ = dual_operand_expression(OPER_LOG_AND, $1, $3); }
    | expression "||" expression        { $$ = dual_operand_expression(OPER_LOG_OR, $1, $3); }
    | expression "^^" expression        { $$ = dual_operand_expression(OPER_LOG_XOR, $1, $3); }
    | expression ">" expression         { $$ = dual_operand_expression(OPER_GREATER, $1, $3); }
    | expression "<" expression         { $$ = dual_operand_expression(OPER_LESS, $1, $3); }
    | expression ">=" expression        { $$ = dual_operand_expression(OPER_GREATER_EQUAL, $1, $3); }
    | expression "<=" expression        { $$ = dual_operand_expression(OPER_LESS_EQUAL, $1, $3); }
    | expression QMARK expression COLON expression { $$ = ternary_expression($1, $3, $5); }
    | INTEGER                           { $$ = integral_expression($1); }
    | ident                             { $$ = symbolic_expression($1); }
    ;

ident
    : IDENTIFIER { $$ = get_identifier($1); free($1); }
    ;

%%


void yyerror(const char *s)
{
    print_error(global_trace, s);
}

