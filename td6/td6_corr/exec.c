#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define HOME_DIR "/home/username"

void ex_ps() {
	execlp("ps", "ps", NULL);
}

void ex_lsr() {
	execlp("ls", "ls", HOME_DIR, NULL);
}

void ex_2find(const int first) {
	if(first)
		execlp("find", "find", HOME_DIR, "-type", "d", NULL);

	char *cmd[] = {"find", HOME_DIR, "-type", "f", NULL};
	execvp("find", cmd);
}

void main() {
	int i;

	if(!fork())
		ex_ps();
	wait(NULL);

	if(!fork())
		ex_lsr();
	wait(NULL);

	for(i = 0; i < 2; ++i)
		if(!fork())
			ex_2find(!i);

	for(i = 0; i < 2; ++i)
		wait(NULL);
}
