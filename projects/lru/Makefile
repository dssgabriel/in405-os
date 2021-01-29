CC=gcc
IN=main.c mem.c se_fichier.c
OFLAG=-O2
CFLAG=-pthread -g3 -o
WFLAG=-Wall

all: compile
	./main thread-config.cfg 

compile:
	$(CC) $(IN) $(OFLAG) $(CFLAG) main #$(WFLAG)

clean:
	rm -f /tmp/pipe_in /tmp/pipe_out
	rm -Rf main
	rm -Rf *.gch
	ls -la

ARCHIVE=td4_dossantos_gabriel
archive: clean
	rm -Rf $(ARCHIVE) $(ARCHIVE).tar
	mkdir $(ARCHIVE)
	cp *.c $(ARCHIVE)
	cp *.h $(ARCHIVE)
	cp Makefile $(ARCHIVE)
	cp README.md $(ARCHIVE)
	cp thread-config.cfg $(ARCHIVE)
	tar -zcvf $(ARCHIVE).tar $(ARCHIVE)
	rm -Rf $(ARCHIVE)
	ls -la
