#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <unistd.h>

int main (int argc, char ** argv) {
	struct tms buf;
	char cmd [1024] = "\0";

	if(argc != 2) {
		printf("Usage : %s rep.\n", argv [0]);
		return -1;
	}

	strcat(cmd, "ls -R ");
	strcat(cmd, argv [1]);

	if(!fork()) {
		system(cmd);
		exit(0);
	}

	wait(NULL);
	times(&buf);

	printf("Temps d'exécution de la commande : %f\n",
		((float)buf.tms_cutime) / sysconf(_SC_CLK_TCK)
		+((float)buf.tms_cstime) / sysconf(_SC_CLK_TCK));

	return 0;	
}
