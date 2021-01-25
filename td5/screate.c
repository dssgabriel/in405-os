#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

void question1 () {
	pid_t a = fork ();

	printf ("Hello World!\n");
	if (a) wait (NULL);
	else exit (0);
}

void question2 () {
	pid_t a = fork ();

	if (a) {
		printf ("Mon PID est %d et celui de mon fils est %d !\n", getpid (), a);
		wait (NULL);
	} else {
		printf ("Mon PID est %d et celui de mon pere est %d !\n", getpid (), getppid () );
		exit (0);
	}
}

void question3 () {
	srand (time (NULL) );

	if (fork () ) {
		int status;
		wait (&status);

		printf ("Valeur choisie par mon fils: %d.\n", WEXITSTATUS (status) );
	} else {
		int a = 1 + (rand () % 50);
		printf ("Valeur choisie aleatoirement: %d.\n", a);
		exit (a);
	}
}

int main (int argc, char ** argv) {
	question1 ();
	question2 ();
	question3 ();

	return 0;
}
