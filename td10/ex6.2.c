#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
	pthread_mutex_t * mut;
	pthread_cond_t * cnd;
	int * cpt;
	int nbT;
} barriere_t;

void * barriere1 (void * a) {
	barriere_t * b = (barriere_t *) a;

	pthread_mutex_lock (b->mut);
	printf ("Entree barriere\n");
	(*b->cpt) --;
	if (*b->cpt != 0)
		pthread_cond_wait (b->cnd, b->mut);
	else
		pthread_cond_broadcast (b->cnd);
	printf ("Sortie barriere\n");
	pthread_mutex_unlock (b->mut);

	return NULL;
}

void question1 () {
	int i, * cpt = malloc (sizeof (int) );
	pthread_t tid [3];
	barriere_t bar [3];
	pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cnd = PTHREAD_COND_INITIALIZER;

	*cpt = 3;

	for (i = 0; i < 3; ++i) {
		bar [i] .mut = &mut;
		bar [i] .cnd = &cnd;
		bar [i] .cpt = cpt;
		pthread_create (tid + i, NULL, barriere1, bar + i);
	}

	for (i = 0; i < 3; ++i)
		pthread_join (tid [i], NULL);

	free (cpt);
}

void * barriere2 (void * a) {
	barriere_t * b = (barriere_t *) a;

	pthread_mutex_lock (b->mut);
	printf ("Entree barriere\n");
	(*b->cpt) --;
	if (*b->cpt != 0)
		pthread_cond_wait (b->cnd, b->mut);
	else
		pthread_cond_broadcast (b->cnd);
	printf ("Sortie barriere\n");
	pthread_mutex_unlock (b->mut);

	return NULL;
}

void question2 (const int n) {
	int i, * cpt = malloc (sizeof (int) );
	pthread_t * tid = malloc (n * sizeof (pthread_t) );
	barriere_t * bar = malloc (n * sizeof (barriere_t) );
	pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cnd = PTHREAD_COND_INITIALIZER;

	*cpt = n;

	for (i = 0; i < n; ++i) {
		bar [i] .mut = &mut;
		bar [i] .cnd = &cnd;
		bar [i] .cpt = cpt;
		pthread_create (tid + i, NULL, barriere2, bar + i);
	}

	for (i = 0; i < n; ++i)
		pthread_join (tid [i], NULL);

	free (bar);
	free (tid);
	free (cpt);
}

void * barriere3 (void * a) {
	int i;
	barriere_t * b = (barriere_t *) a;

	for (i = 0; i < 3; ++i) {
		pthread_mutex_lock (b->mut);
		printf ("Entree barriere %d\n", i);
		(*b->cpt) --;
		if (*b->cpt != 0)
			pthread_cond_wait (b->cnd, b->mut);
		else {
			(*b->cpt) = b->nbT;
			pthread_cond_broadcast (b->cnd);
		}
		printf ("Sortie barriere %d\n", i);
		pthread_mutex_unlock (b->mut);
	}

	return NULL;
}

void question3 (const int n) {
	int i, * cpt = malloc (sizeof (int) );
	pthread_t * tid = malloc (n * sizeof (pthread_t) );
	barriere_t * bar = malloc (n * sizeof (barriere_t) );
	pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cnd = PTHREAD_COND_INITIALIZER;

	*cpt = n;

	for (i = 0; i < n; ++i) {
		bar [i] .mut = &mut;
		bar [i] .cnd = &cnd;
		bar [i] .cpt = cpt;
		bar [i] .nbT = n;
		pthread_create (tid + i, NULL, barriere3, bar + i);
	}

	for (i = 0; i < n; ++i)
		pthread_join (tid [i], NULL);

	free (bar);
	free (tid);
	free (cpt);
}

int main (int argc, char ** argv) {
	question1 ();
	question2 (5);
	question3 (7);

	return 0;
}
	