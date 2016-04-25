#include <stdint.h>
#define BUFFSIZE 1000
#define true 1
#define false 0

typedef uint8_t bool;
typedef uint8_t byte;

typedef struct {
	char op;
	unsigned long long int address;
	unsigned int bytesize;
} ref;

typedef struct {
    ref * q[BUFFSIZE+1];		/* body of queue */
    int count;          
    int tail;
    int head;
} fifo;

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