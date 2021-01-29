CC = gcc
CFLAGS = -Wall -Wextra -g3
CLIB = -pthread

build: target
	$(CC) $(CFLAGS) $(CLIB) src/threads.c -o target/threads
	$(CC) $(CFLAGS) $(CLIB) src/reduc.c -o target/reduc

threads: target
	$(CC) $(CFLAGS) $(CLIB) src/threads.c -o target/threads
	target/threads

reduc: target
	$(CC) $(CFLAGS) $(CLIB) src/reduc.c -o target/reduc
	target/reduc 10 1000 /

vg_threads: target
	$(CC) $(CFLAGS) $(CLIB) src/threads.c -o target/threads
	valgrind --leak-check=full -v target/threads

vg_reduc: target
	$(CC) $(CFLAGS) $(CLIB) src/reduc.c -o target/reduc
	valgrind --leak-check=full -v target/reduc 10 1000 /

target:
	mkdir -p target/

clean:
	rm -Rf target/
