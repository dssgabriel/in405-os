#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void pere(const pid_t a) {
	sleep(3);
	kill(a, SIGSTOP);
	sleep(3);
	kill(a, SIGCONT);
}

void fils() {
	int i = 1;

	while(i < 6) {
		sleep(1);
		printf("Attente à %dsec.\n", i);
		i++;
	}

	exit(0);
}

int main(int argc, char ** argv) {
	pid_t a;
	if(a = fork())
		pere (a);
	else 
		fils();

	wait(NULL);

	return 0;
}
