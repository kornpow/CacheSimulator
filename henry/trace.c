#include <stdio.h>
#include <stdlib.h>

char op;
unsigned long long int address;
unsigned int bytesize;

int main(){
	printf("hello\n");
		while(scanf("%c %llu %d\n",&op,&address,&bytesize)==3)
			printf("bye\n");
			printf("%c %llu %d\n",op,address,bytesize);
}