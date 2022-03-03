
%{

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.tab.h"
#include "trace.h"

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

".address"           { return D_ADDR; }
".byte"              { return D_BYTE; }
".word"              { return D_WORD; }
".include"           { return D_INCLUDE; }
".include_once"      { return D_INCLUDE_ONCE; }
".include_binary"    { return D_INCLUDE_BIN; }

0x[0-9a-fA-F]+  {
    yylval.integer = strtol(yytext, NULL, 16);
    return INTEGER; 
}

0b[01]+  {
    return INTEGER;
}

[0-9]+  {
    yylval.integer = atoi(yytext);
    return INTEGER; 
}

\".\"  {
    return STRING; 
}

[a-zA-Z_][a-zA-Z0-9_]*  {
    yylval.string = strdup(yytext);
    return IDENTIFIER;
}

\'.\' {
    yylval.string = strdup(yytext);
    return CHARACTER;
}

\'\\.\' {
    yylval.string = strdup(yytext);
    return CHARACTER;
}

\n { 
    global_trace.line += 1;
    global_trace.column = 1;
    return NEWLINE; 
}

\;.*         { ; }
[ \t\r]+     { ; }

.            { printf("Invalid token.\n"); }

%%

