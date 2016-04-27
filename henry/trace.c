#include <stdio.h>
#include <stdlib.h>

char op;
unsigned long int address;
unsigned int bytesize;

int main(){
	printf("hello\n");
		while(scanf("%c %ld %u\n",&op,&address,&bytesize)==3) {
			printf("bye\n");
			printf("%c %ld %u\n",op,address,bytesize);
		}
}

//7f | 81 | ce | 22 | 06 | b0