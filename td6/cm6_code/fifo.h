#ifndef _H_FIFO_
#define _H_FIFO_

struct fifo;

struct fifo *fifo_init();
void fifo_fini(struct fifo *f);
void fifo_push(struct fifo *f, const int val);
int fifo_pull(struct fifo *f);
int fifo_peek(struct fifo *f);
int fifo_size(struct fifo *f);

#endif