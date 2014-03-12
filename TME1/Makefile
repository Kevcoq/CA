all : 
	bison -d calc.y
	flex calc.lex 
	gcc -c *.c
	gcc -o calc *.o -lfl -lm

clean :
	rm -f calc *~ *.o *.h *.c