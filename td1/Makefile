all: testLexicon
	./testLexicon

testLexicon: testLexicon.o lexicon.o 
	gcc -Wall -o testLexicon testLexicon.o lexicon.o

testLexicon.o: testLexicon.c
	gcc -Wall -c -g testLexicon.c

lexicon.o: lexicon.c lexicon.h
	gcc -Wall -c -g lexicon.c

leaks: testLexicon
	valgrind --leak-check=full --show-leak-kinds=all ./testLexicon

clean:
	rm -f *.o
# 	rm -f *.h.gch
	rm -f testLexicon
	ls -l
