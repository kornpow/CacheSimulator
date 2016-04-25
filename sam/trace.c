#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "fifo.h"

char op;
unsigned long long int address;
unsigned int bytesize;

pthread_t pth;	// this is our thread identifier
pthread_mutex_t lock;

void pullData(fifo * traces);
void popData(fifo * traces);

int main(){
	fifo traces; //Make the FIFO queue which will hold onto the instruction traces
	init_fifo(&traces); //Initialize the FIFO queue variables
	

	pthread_mutex_init(&lock, NULL);

	pthread_create(&pth, NULL, pullData, &traces);
	

	int i = 0;
	while(i < 100)
	{
		usleep(1);
		int count = traces.count;
		printf("The size of the buffer: %d\n",count);
		printf("main is running...\n");
		++i;
	}
	pthread_create(&pth, NULL, popData, &traces);
	

	printf("Reading In Traces:\n");

	printf("main waiting for thread to terminate...\n");
	pthread_join(pth,NULL);

}

void pullData(fifo * traces) {
	while(scanf("%c %Lx %d\n",&op,&address,&bytesize)==3) {
		printf("%c %Lx %d\n",op,address,bytesize);
		ref *newref = (ref *) malloc(24); //Allocate memory for the memory reference
		newref->op = op;
		newref->address = address;
		newref->bytesize = bytesize;
		pthread_mutex_lock(&lock);
		enqueue(&traces, newref);
		pthread_mutex_unlock(&lock);	
	}	
}

void popData(fifo * traces) {
	while(!empty(&traces)) {
		pthread_mutex_lock(&lock);
		ref *tmpref = dequeue(&traces);
		pthread_mutex_unlock(&lock);
		printf("Popping buffers all day.\n");
		printf("%c %Lx %d\n",tmpref->op,tmpref->address,tmpref->bytesize);
		free(tmpref);
	}
}