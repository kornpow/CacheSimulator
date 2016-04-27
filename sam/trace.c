#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

// #include "inbuf.h"

#define BSIZE 100


// fifo traces; //Make the FIFO queue which will hold onto the instruction traces

char op;
unsigned long long int address;
unsigned int bytesize;

// ****** Structure Declaration! *********

typedef struct {
	char op;
	unsigned long long int address;
	unsigned int bytesize;
} ref;


// ****** END Structure Declaration! *********


ref inbuffer[BSIZE];


ref bundleRef(char op, unsigned long long int address, unsigned int bytesize);

int main(){
	// init_fifo(&traces); //Initialize the FIFO queue variables
	// init_inbuf(&traces);
	printf("Reading In Traces:\n");
	// pullData(&traces);
	// popData(&traces);
	int count = fillBuffer();

	printf("Read in %d elements from the stream.\n",(count+1));

	for(int i = 0; i < count; i++) {
		printf("Data!:\n");
		printf("Op: %c, address: %Lx, bytesize: %d\n",inbuffer[i].op, inbuffer[i].address, inbuffer[i].bytesize);
	}

	printf("main waiting for input buffer thread to terminate...\n");

	return 0;
}

ref bundleRef(char op, unsigned long long int address, unsigned int bytesize) {
	ref newRef;
	newRef.op = op;
	newRef.address = address;
	newRef.bytesize = bytesize;
	return newRef;
}

int fillBuffer() {
		int fill = 0;
	while(scanf("%c %Lx %d\n",&op,&address,&bytesize)==3) {
		inbuffer[fill] = bundleRef(op, address, bytesize);
		fill++;
	}
	return (fill-1);
}

// void pullData(fifo * traces) {
// 	while(scanf("%c %Lx %d\n",&op,&address,&bytesize)==3) {
// 		printf("%c %Lx %d\n",op,address,bytesize);
// 		ref * newref = (ref *) malloc(sizeof(ref)); //Allocate memory for the memory reference
// 		newref->op = op;
// 		newref->address = address;
// 		newref->bytesize = bytesize;
// 		enqueue(&traces, &newref);
// 	}	
// }

// void popData(fifo * traces) {
// 	printf("Checking if buffer is empty\n");
// 	while(!empty(&traces)) {
// 		printf("Not Empty!\n");
// 		ref *tmpref = dequeue(&traces);
// 		printf("Popping buffers all day.\n");
// 		printf("%c %Lx %d\n",tmpref->op,tmpref->address,tmpref->bytesize);
// 		free(tmpref);
// 	}
// }