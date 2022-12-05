
all:
	flex -o lex.yy.c lexer.l
	bison -d parser.y -o parser.tab.c --header=parser.tab.h
	gcc -g -o spas main.c node.c expression.c lex.yy.c parser.tab.c instruction.c symbol.c assembler.c trace.c error.c parse.c instruction_set.c context.c identifier.c stdlib.c
 
install:
	cp spas $(HOME)/.local/bin/spas

clean:
	rm spas lex.yy.c parser.tab.c parser.tab.h

uninstall:
	rm $(HOME)/.local/bin/spas

