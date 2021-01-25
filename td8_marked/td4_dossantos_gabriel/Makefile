all: compile
	./scenario-test.sh

server: compile
	./million server lottery.cfg

client: compile
	./million client 2 5 9 12 15

compile: se_fichier.o million.o
	gcc -Wall -g -o million million.o se_fichier.o

se_fichier.o: se_fichier.c se_fichier.h
	gcc -Wall -g -c se_fichier.c se_fichier.h

million.o: million.c 
	gcc -Wall -g -c million.c

clean:
	rm -f *.o
	rm -f *.gch
	rm -f million
	ls -l


ZIPNAME=td4_dossantos_gabriel
archive: clean
	rm -rf ${ZIPNAME} ${ZIPNAME}.tar
	mkdir ${ZIPNAME}
	cp *.c *.h *.sh *.cfg ${ZIPNAME}
	cp Makefile ${ZIPNAME}
	cp README.md ${ZIPNAME}
	tar -zcvf ${ZIPNAME}.tar ${ZIPNAME}
	rm -rf ${ZIPNAME}
	ls -l