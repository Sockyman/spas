
all:
	flex -o lex.yy.c lexer.l 
	bison -d parser.y -o parser.tab.c --header=parser.tab.h
	gcc -g -o spasm main.c syntax_tree.c node.c expression.c lex.yy.c parser.tab.c instruction.c symbol.c resolve.c assembler.c trace.c error.c

