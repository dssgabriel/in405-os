#include <stdio.h>

#include "scheduler.h"

struct process pids[5] = {
	{0,  0, 10, 0},
	{1,  2,  6, 0},
	{2,  8, 18, 0},
	{3, 10, 12, 0},
	{4, 14,  2, 0}
};

void main() {
	int choice;

	printf("Which version? [1-3]\n"
		   "\t1 - fifo\n"
		   "\t2 - sjf\n"
		   "\t3 - rr\n");

	scanf("%d", &choice);

	if (choice < 1 || choice > 3) {
		printf("invalid entry\n");
		return;
	}

	algo_tab[choice - 1](pids, 5);
}