#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int question1() {

	char *arg[] = {"ps -fux", NULL};

	if(execv("/bin/ps", arg) == -1) {
		perror("error in execv");
		return errno;
	}

	return 0;
}

int question2(char *path) {

	if(!path) {
		perror("error: no path given\n");
		return errno;
	}

	char *arg[] = {"ls", "-lahR", path, NULL};

	if(execv("/bin/ls", arg) == -1) {
		perror("error in execv");
		return errno;
	}

	return 0;
}

int question3(char *path, char *type) {

	if(strcmp(path, "~"))
		path = "/home";

	char *arg = {"find", "-type", type, path, NULL};

	if(execlp("find", arg) == -1) {
		perror("error in execlp");
		return errno;
	}

	return 0;
}

int cmdexec() {

	char *who = "father"
	pid_t a;

	switch(a = fork()) {
		case -1:
			perror("error in fork");
			return errno;
		case 0:
			who = "child1";
		default:
			a = fork();
			who = "child2";
	}

	return 0;
}

int main(int argc, char **argv) {

	// question1();
	// question2(argv[1]);
	question3(argv[2], "d");
	question3(argv[3], "f");
	
	return 0;
}
