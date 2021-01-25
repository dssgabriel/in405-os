#ifndef _H_SCHEDULER_
#define _H_SCHEDULER_

struct process {
	int id;
	int arrival_time;
	int exec_time;
	int elapsed_time;
};

void algo_fifo(struct process *pids, const int n_pids);
void algo_sjf(struct process *pids, const int n_pids);
void algo_rr(struct process *pids, const int n_pids);

extern void (*algo_tab[])(struct process *, const int);

#endif