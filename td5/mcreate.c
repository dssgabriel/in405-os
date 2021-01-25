#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

void question1 (int m, int n) {
	int i, j, t = 1, status;

	for (i = 0; i < m; ++i) {
		for (j = 0; j < n; ++j) {
			if (! fork () ) exit (t);
		}
	}

	for (i = 0; i < m; ++i) {
		for (j = 0; j < n; ++j) {
			wait (&status);
			t += WEXITSTATUS (status);
		}
	}

	printf ("Nombre de processus créés : %d!\n", t);
}

void question2 (int m, int n) {
	int i, j, t = 1, status;

	for (i = 0; i < m; ++i) {
		if (fork () ) continue;
		for (j = 0; j < n; ++j) {
			if (! fork () ) exit (t);
		}

		for (j = 0; j < n; ++j) {
			wait (&status);
			t += WEXITSTATUS (status);
		}

		exit (t);
	}

	for (i = 0; i < m; ++i) {
		wait (&status);
		t += WEXITSTATUS (status);
	}

	printf ("Nombre de processus créés : %d!\n", t);
}

// Version itérative de la question 3
void question3 (int m, int n) {
	pid_t a;
	int i, j, t = 1, status;

	for (i = 0; i < m; ++i) {
		for (j = 0; j < n; ++j) {
			// Si fils, ne créé pas d'autres processus à cette étape
			if (! (a = fork () ) ) break;
		}

		// Si père
		if (a) {
			// Attente des fils
			for (j = 0; j < n; ++j) {
				wait (&status);
				t += WEXITSTATUS (status);
			}

			// Si ce n'est pas le proc principal, alors exit
			if (i != 0)
				exit (t);
			else break;
		}
		// Si fils et que dernière étape, alors exit
		else if (i == m-1) exit (t);
	}

	// Affichage pour le proc principal
	printf ("Nombre de processus créés : %d!\n", t);
}

// Version récursive de la question 3
// firstCall est vrai lors de l'entrée de la fonction récursive, puis faux :
// utilisé pour l'affichage par le proc principal
void question3R (int m, int n, int firstCall) {
	int i, t = 1, status;

	if (!m) exit (t);

	for (i = 0; i < n; ++i)
		if (! fork () ) question3R (m-1, n, 0);

	for (i = 0; i < n; ++i) {
		wait (&status);
		t += WEXITSTATUS (status);
	}

	if (firstCall)
		printf ("Nombre de processus créés : %d!\n", t);
	else
		exit (t);
}

int main (int argc, char ** argv) {
	if (argc != 3) {
		printf ("Usage : %s m n.\n", argv [0]);
		return -1;
	}

	question1 (atoi (argv [1]), atoi (argv [2]));
	question2 (atoi (argv [1]), atoi (argv [2]));
	question3 (atoi (argv [1]), atoi (argv [2]));
	question3R (atoi (argv [1]), atoi (argv [2]), 1);

	return 0;
}
