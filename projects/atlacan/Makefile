all: compile
	./a.out

compile:
	gcc -g main.c atlacan.c

valgrind: compile
	valgrind --leak-check=full --show-leak-kinds=all ./a.out

gdb: compile
	gdb ./a.out

clean:
	rm -f *.out
	ls -l


ZIPNAME=TD4-G_SCREVE-G_DOS_SANTOS	
archive: clean
	rm -rf ${ZIPNAME}
	mkdir ${ZIPNAME}
	cp *.c ${ZIPNAME}
	cp *.h ${ZIPNAME}
	cp Makefile ${ZIPNAME}
	cp README.md ${ZIPNAME}
	zip -r $(ZIPNAME).zip $(ZIPNAME)
	rm -r ${ZIPNAME}
	ls -l
	