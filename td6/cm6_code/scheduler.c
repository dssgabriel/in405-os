#include "scheduler.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fifo.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define QUANTUM 2

void (*algo_tab[3])(struct process *, const int) = {
	algo_fifo, algo_sjf, algo_rr
};

void _insert_in_order(struct fifo *arrival_queue,
					  struct process *pids, const int n_pids)
{
	int *checked;
	int i, j;

	checked = calloc(n_pids, sizeof(*checked));

	for (i = 0; i < n_pids; ++i) {
		int min_arrival = INT_MAX;
		int min_ind = -1;

		for (j = 0; j < n_pids; ++j) {
			if (checked[j])
				continue;

			if (pids[j].arrival_time >= min_arrival)
				continue;

			min_arrival = pids[j].arrival_time;
			min_ind = j;
		}

		checked[min_ind] = 1;
		fifo_push(arrival_queue, min_ind);
	}
}

void _exec_process(struct process *pid, int *current_date,
				   const int elapsed_time)
{
	printf("executing P#%d during [%d, %d]",
		   pid->id, *current_date, *current_date + elapsed_time - 1);

	pid->elapsed_time += elapsed_time;
	*current_date += elapsed_time;

	if (pid->elapsed_time == pid->exec_time)
		printf(", terminated");
	printf("\n");
}

void algo_fifo(struct process *pids, const int n_pids)
{
	int current_date = 0;
	int i;

	while (1) {
		int min_arrival = INT_MAX;
		int to_do = -1;

		for (i = 0; i < n_pids; ++i) {
			if (pids[i].elapsed_time == pids[i].exec_time)
				continue;

			if (pids[i].arrival_time < min_arrival) {
				to_do = i;
				min_arrival = pids[i].arrival_time;
			}
		}

		if (to_do < 0)
			break;

		current_date = MAX(current_date, min_arrival);
		_exec_process(pids + to_do, &current_date, pids[to_do].exec_time);
	}
}

void algo_sjf(struct process *pids, const int n_pids)
{
	int current_date = 0;
	int i;

	while (1) {
		int min_arrival = INT_MAX;
		int min_exec = INT_MAX;
		int to_do = -1;

		for (i = 0; i < n_pids; ++i) {
			if (pids[i].elapsed_time == pids[i].exec_time)
				continue;

			if (pids[i].arrival_time > current_date) {
				if (min_exec == INT_MAX && pids[i].arrival_time > min_arrival) {
					to_do = i;
					min_arrival = pids[i].arrival_time;
				}

				continue;
			}

			if (pids[i].exec_time < min_exec) {
				to_do = i;
				min_exec = pids[i].exec_time;
			}
		}

		if (to_do < 0)
			break;

		if (min_exec == INT_MAX)
			current_date = MAX(current_date, min_arrival);
		_exec_process(pids + to_do, &current_date, pids[to_do].exec_time);
	}
}

void algo_rr(struct process *pids, const int n_pids)
{
	struct fifo *waiting_queue;
	struct fifo *arrival_queue;
	int current_date;
	int i;

	arrival_queue = fifo_init();
	waiting_queue = fifo_init();

	_insert_in_order(arrival_queue, pids, n_pids);
	i = fifo_pull(arrival_queue);
	current_date = pids[i].arrival_time;
	fifo_push(waiting_queue, i);

	while (1) {
		int to_do = fifo_pull(waiting_queue);

		current_date = MAX(current_date, pids[to_do].arrival_time);
		_exec_process(pids + to_do, &current_date, QUANTUM);

		if (fifo_size(arrival_queue)) {
			while (1) {
				int next = fifo_peek(arrival_queue);

				if (pids[next].arrival_time < current_date) {
					fifo_push(waiting_queue, fifo_pull(arrival_queue));
					if (!fifo_size(arrival_queue))
						break;
				} else {
					break;
				}
			}
		}

		if (pids[to_do].elapsed_time < pids[to_do].exec_time)
			fifo_push(waiting_queue, to_do);

		if (!fifo_size(waiting_queue) && fifo_size(arrival_queue))
			fifo_push(waiting_queue, fifo_pull(arrival_queue));

		if (!fifo_size(waiting_queue))
			break;
	}

	fifo_fini(arrival_queue);
	fifo_fini(waiting_queue);
}