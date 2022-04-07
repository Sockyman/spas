
%{

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.tab.h"
#include "trace.h"
#include "error.h"

%}

%option noyywrap 

%%

"="    { return ASSIGN; }
":"    { return COLON; }
","    { return COMMA; }
"("    { return PAREN_OPEN; }
")"    { return PAREN_CLOSE; }
"["    { return SQUARE_OPEN; } 
"]"    { return SQUARE_CLOSE; }
"x"    { return XREG; }
"y"    { return YREG; }
"#"    { return HASH; }

"+"    { return ADD; }
"-"    { return SUBTRACT; }
"*"    { return MULTIPLY; }
"/"    { return DIVIDE; }

"&"    { return BIT_AND; }
"|"    { return BIT_OR; }
"^"    { return BIT_XOR; }
"~"    { return BIT_NOT; }

"<<"    { return SHIFT_L; }
">>"    { return SHIFT_R; }

"&&"    { return LOG_AND; }
"||"    { return LOG_OR; }
"^^"    { return LOG_XOR; }

"=="    { return EQUAL; }
"!="    { return NOT_EQUAL; }
">"     { return GREATER; }
"<"     { return LESS; }
">="    { return GREATER_EQUAL; }
"<="    { return LESS_EQUAL; }

"!"     { return LOG_NOT; }

"%address"           { return D_ADDR; }
"%byte"              { return D_BYTE; }
"%word"              { return D_WORD; }
"%align"             { return D_ALIGN; }
"%include"           { return D_INCLUDE; }
"%include_once"      { return D_INCLUDE_ONCE; }
"%include_binary"    { return D_INCLUDE_BIN; }

"%section"          { return D_SECTION; }
"%res"              { return D_RESERVE; }

"." {
    return DOT;
}

0x[0-9a-fA-F]+  {
    yylval.integer = strtol(yytext, NULL, 16);
    return INTEGER; 
}

0b[01]+  {
    int val = 0b0;
    for (int i = 2; i < strlen(yytext); ++i)
    {
        val = val << 1;
        if (yytext[i] == '1')
        {
            ++val;
        }
    }

    yylval.integer = val;
    return INTEGER;
}

[0-9]+  {
    yylval.integer = atoi(yytext);
    return INTEGER; 
}

\'.\' {
    yylval.integer = yytext[1];
    return INTEGER;
}

\"([^\"])*\" {
    yylval.string = malloc(strlen(yytext) - 1);
    memcpy(yylval.string, yytext + 1, strlen(yytext) - 2);
    yylval.string[strlen(yytext) - 1] = 0;
    return STRING;
}

\.*[a-zA-Z_][a-zA-Z0-9_]*(\.[a-zA-Z_][a-zA-Z0-9_]*)*  {
    yylval.string = strdup(yytext);
    return IDENTIFIER;
}

\n { 
    global_trace->line += 1;
    global_trace->column = 1;
    return NEWLINE; 
}

\;.*         { ; }
[ \t\r]+     { ; }

.            { print_error(NULL, "Invalid token: %s.", yytext); }

%%

