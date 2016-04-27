#include <stdint.h>

#define true 1
#define false 0

typedef uint8_t bool;
typedef uint8_t byte;


typedef struct {
    ref q[BUFFSIZE];		/* body of queue */
    int count;          
    int tail;
    int head;
} fifo;

typedef struct {
	char op;
	unsigned long long int address;
	unsigned int bytesize;
} ref;


typedef struct {
	byte index;
	byte tag;
	bool valid;
	long long int data;
} c_entry;

void init_fifo(fifo *q);
int enqueue(fifo *q, ref *data);
ref * dequeue(fifo *q);
int empty(fifo *q);
int size(fifo *q);

void init_inbuf(inbuf * in);
int