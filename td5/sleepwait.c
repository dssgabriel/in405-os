#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

void fils (int a) {
	sleep (a);
	printf ("%d [%d]\n", getpid (), a);

	exit (0);
}

int main (int argc, char ** argv) {
	int i;
	pid_t a;

	srand (time (NULL) );

	for (i = 0; i < 10; ++i) {
		// Génération dans le processus pere pour sortir les nombres de la
		// même graine
		int r = 1 + (rand () % 10);
		if (! fork () ) fils (r);
	}

	for (i = 0; i < 10; ++i) {
		a = wait (NULL);
		printf ("%d est fini\n", a);
	}

	return 0;
}
