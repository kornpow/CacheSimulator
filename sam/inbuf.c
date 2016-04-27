#include "inbuf.h"


// void init_inbuf(inbuf * in) {
// 	in->size = 0;
// 	in->rbuff = (ref *) malloc(sizeof(ref)*BUFFSIZE);
// 	if(!in->rbuff) {
// 		printf("Malloc failed in inbuf\n");
// 	}
// }
// int push(inbuf * in, char op, unsigned long long int address; unsigned int bytesize) {
// 	if(in->size == BUFFSIZE) {
// 		printf("Buffer Full!\n");
// 		return 0;
// 	}
// 	in->size++; //Increment the size of the buffer
// 	in->rbuff[in->size-1]->op = op;
// 	in->rbuff[in->size-1]->address = address;
// 	in->rbuff[in->size-1]->bytesize = bytesize;
// 	return 1;
// }
// ref * pop(inbuf * in) {
// }


