#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <cstdlib.h>

unsigned int cconfig[12];
unsigned int mconfig[4];

void usage();
int readConfig(char *cc, char *mc);


int main(int argc, char ** argv) {
	printf("Number of parameters: %d\n",argc);
	if(argc != 3)
	{
		usage();
		return -1;
	}

	for(int i = 0; i < 12; i++)
		cconfig[i] = 0;
	for(int i = 0; i < 4; i++)
		mconfig[i] = 0;

	readConfig(argv[1],argv[2]);

	

	return 0;
}

void usage() {
	printf("Usage: <progname> <cache_config> <memory_config>\n");
}

int readConfig(char *cache, char *mem) {
	FILE *cc = fopen(cache,"r");
	if ( cc == 0 )
	{
		printf( "Could not open cache file\n" );
		return -1;
	}
	FILE *mc = fopen(mem,"r");
	if ( mc == 0 )
	{
		printf( "Could not open memory file\n" );
		return -1;
	}
	int x;
	char cbuf[15][20];
	char mbuf[5][20];
	char buf[20] = {0};

	//Get L1
	fgets(buf,20,cc);
	for(int j = 0; j <20; j++) {
		printf("%c",buf[j]);
	}

            /* read one character at a time from file, stopping at EOF, which
               indicates the end of the file.  Note that the idiom of "assign
               to a variable, check the value" used below works because
               the assignment statement evaluates to the value assigned. */


    int cnum = 0;
    for(int i = 0; i < 16; i++) {
    	fgets(cbuf[i],20,cc);
    	int w = 0;
    	while(cbuf[i][w] != 0) {
    		printf("%c",cbuf[i][w]);
    		w++;
    	}
    	const char * cstr = strstr(cbuf[i],":");
    	if(cstr) {
    		printf("\n");
    		int w = 0;
    		int result = atoi(cstr+1);
    		printf("The integer result is: %d.\n",result);
    		cconfig[cnum] = result;
    		cnum++;
    	}
    }
    printf("\n\n\n");
    printf("Here is the passed in parameter values:\n");
    for(int q = 0; q < 12; q++) {
    	printf("%d: %d\n", q, cconfig[q]);
    }


    int mnum = 0;
    for(int i = 0; i < 6; i++) {
    	fgets(mbuf[i],20,mc);
    	int w = 0;
    	while(mbuf[i][w] != 0) {
    		printf("%c",mbuf[i][w]);
    		w++;
    	}
    	const char * mstr = strstr(mbuf[i],":");

    	if(mstr) {
    		printf("\n");
    		int w = 0;
    		int result = atoi(mstr+1);
    		printf("The integer result is: %d.\n",result);
    		mconfig[mnum] = result;
    		mnum++;
    	}
    }
    printf("\n\n\n");
    printf("Here is the passed in parameter values:\n");
    for(int q = 0; q < 4; q++) {
    	printf("%d: %d\n", q, mconfig[q]);
    }
               
	fclose(cc);
	fclose(mc);

	return 0;

}