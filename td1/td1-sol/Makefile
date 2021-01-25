all: test exec

test: compile
	./test_lexc

exec: compile
	./lexc

compile:
	gcc -DTEST_LEXC -g3 test_lexc.c lexc.c lexc_err.c -o test_lexc
	gcc -g3 test_lexc.c lexc.c lexc_err.c -o lexc

clean:
	rm -Rf *~ test_lexc lexc
	ls -l