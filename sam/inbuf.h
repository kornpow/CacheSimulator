#include <stdint.h>
#define BUFFSIZE 1000


// void init_inbuf(inbuf * in);
// int push(inbuf * in, char op, unsigned long long int address; unsigned int bytesize);
// ref * pop(inbuf * in);

typedef struct {
	char op;
	unsigned long long int address;
	unsigned int bytesize;
} ref;

typedef struct {
	int size;
	ref rbuff[BUFFSIZE]; //An array of references of the size of BUFFSIZE
} inbuf;