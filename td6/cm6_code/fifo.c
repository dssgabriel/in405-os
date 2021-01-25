#include "fifo.h"

#include <assert.h>
#include <stdlib.h>

struct fifo_ent {
	int val;
	struct fifo_ent *next;
};

struct fifo {
	struct fifo_ent *start;
	struct fifo_ent *end;
};

struct fifo *fifo_init()
{
	struct fifo *f = malloc(sizeof(f));

	assert(f);
	f->start = NULL;
	f->end = NULL;

	return f;
};

void fifo_fini(struct fifo *f)
{
	struct fifo_ent *p;

	assert(f);
	p = f->start;

	while (p) {
		struct fifo_ent *n = p->next;
		free(p);
		p = n;
	}

	free(f);
}

void fifo_push(struct fifo *f, const int val)
{
	struct fifo_ent *p;

	assert(f);
	p = f->end;

	f->end = malloc(sizeof(*f->end));
	assert(f->end);
	f->end->val = val;
	f->end->next = NULL;

	if (p)
		p->next = f->end;
	else
		f->start = f->end;
}

int fifo_pull(struct fifo *f)
{
	struct fifo_ent *p;
	int val;

	assert(f);
	assert(f->start);
	p = f->start;
	f->start = f->start->next;
	val = p->val;
	free(p);

	if (!f->start)
		f->end = NULL;

	return val;
}

int fifo_peek(struct fifo *f)
{
	assert(f);
	assert(f->start);

	return f->start->val;
}

int fifo_size(struct fifo *f)
{
	struct fifo_ent *p;
	int size = 0;

	assert(f);

	p = f->start;
	while (p) {
		++size;
		p = p->next;
	}

	return size;
}