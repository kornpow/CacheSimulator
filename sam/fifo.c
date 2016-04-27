#include <stdio.h>
#include "fifo.h"



void init_fifo(fifo *q) {
	q->count = 0;
	q->head = 0;
	q->tail = 0;
	// q->q = malloc(sizeof(ref)*BUFFSIZE);
}

int enqueue(fifo *q, ref *data) {
	if(q->count >= 1000) {
		printf("Buffer is full!!!\n");
		return 0;
	}
	else {
		q->q[q->tail] = data; //Save data address in the tail
		q->tail = (q->tail + 1)%BUFFSIZE; //Increment tail
		q->count = q->count + 1; //Add another reference onto the queue
	}
}
ref * dequeue(fifo *q) {
	q->count = q->count - 1; //Add another reference onto the queue
	ref * trace = q->q[q->head];
	q->head = (q->head+1)%BUFFSIZE; //Shift up the head
	return trace;
}

int empty(fifo *q)
{
        if (q->count <= 0) return (1);
        else return (0);
}

int size(fifo *q) {
	return q->count;
}