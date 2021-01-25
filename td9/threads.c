#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>


void *q9_1(void *arg) 
{
	char *str = (char *)arg;
	printf("%s\n", str);

	return NULL;
}

void *q9_2(void *arg) 
{
	int *rnd = (int *)arg;
	printf("%d\n", *rnd);

	return NULL;
}

void *q9_3(void *arg)
{
	srand(getpid());
	int rnd = rand() * rand();

	return rnd;
}

int main(int argc, char **argv)
{
	pthread_t tid9_1;
	pthread_create(&tid9_1, NULL, q9_1, "Hello world");
	pthread_join(tid9_1, NULL);

	pthread_t tid9_2;
	srand(getpid());
	int rnd = rand();
	pthread_create(&tid9_2, NULL, q9_2, &rnd);
	pthread_join(tid9_1, NULL);

	pthread_t tid9_3;
	int rnd2;
	rnd2 = pthread_create(&tid9_3, NULL, q9_3, &rnd2);
	printf("%d\n", rnd2);
	pthread_join(tid9_3, NULL);

	return 0;
}			