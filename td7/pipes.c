#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 16

void q1Anonymous() {
	int pipefd[2];
	char buf[BUFFER_SIZE];

	pipe(pipefd);
	if(fork()) {
		strcpy(buf, "Hello world!");
		write(pipefd[1], buf, BUFFER_SIZE);

		close(pipefd[0]); close(pipefd[1]);

		wait(NULL);
	} 
	else {
		read(pipefd[0], buf, BUFFER_SIZE);
		printf("%s\n", buf);

		close(pipefd[0]); close(pipefd[1]);
		exit(0);
	}
}

void q1Named() {
	int fd;
	char buf[BUFFER_SIZE];

	mkfifo("/tmp/tube", 0600);
	if(fork()) {
		fd = open("/tmp/tube", O_WRONLY);
		strcpy(buf, "Here is Brian!");

		write(fd, buf, BUFFER_SIZE);

		close(fd);
		wait(NULL);
	}
	else {
		fd = open("/tmp/tube", O_RDONLY);

		read(fd, buf, BUFFER_SIZE);
		printf("%s\n", buf);

		close(fd);
		exit(0);
	}
	unlink("/tmp/tube");
}

void q2Anonymous() {
	int pipefd[2], number1, number2;

	pipe(pipefd);
	if(fork()) {
		number1 = rand() % 1000;
		number2 = rand() % 10;

		write(pipefd[1], &number1, sizeof(int));
		write(pipefd[1], &number2, sizeof(int));

		close(pipefd[0]); close(pipefd[1]);

		wait(NULL);
	}
	else {
		read(pipefd[0], &number1, sizeof(int));
		read(pipefd[0], &number2, sizeof(int));

		printf("(%d, %d)\n", number1, number2);

		close(pipefd[0]); close(pipefd[1]);
		exit (0);
	}
}

void q2Named() {
	int fd, number1, number2;
	char buf[BUFFER_SIZE];

	mkfifo("/tmp/tube", 0600);
	if(fork()) {
		fd = open("/tmp/tube", O_WRONLY);
		number1 = rand() % 1000;
		number2 = rand() % 10;

		write(fd, &number1, sizeof(int));
		write(fd, &number2, sizeof(int));

		close(fd);
		wait(NULL);
	}
	else {
		fd = open("/tmp/tube", O_RDONLY);
		read(fd, &number1, sizeof(int));
		read(fd, &number2, sizeof(int));

		printf("(%d, %d)\n", number1, number2);

		close(fd);
		exit(0);
	}
	unlink("/tmp/tube");
}

int main(int argc, char ** argv) {
	srand(time(NULL));

	q1Anonymous();
	q2Anonymous();

	q1Named();
	q2Named();

	return 0;
}
