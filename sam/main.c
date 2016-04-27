#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
// #include <cstdlib.h>

#define BSIZE 100

unsigned int cconfig[12];
unsigned int mconfig[4];

char op;
unsigned long long int address;
unsigned int bytesize;

// ***** Structure Declaration *****

typedef struct c_entry{
    uint32_t index;
    uint8_t valid;
    uint8_t tag;
    long long int data;
} c_entry;

typedef struct {
    char op;
    unsigned long long int address;
    unsigned int bytesize;
} ref;

ref inbuffer[BSIZE]; //Buffer to hold cache trace entries.

// *****  END Structure Declaration *****
// *****  L1, L2, And LRU Cache Pointers
c_entry * l1cache;
c_entry * l2cache;

void usage();
int readConfig(char *cc, char *mc);
int computeCost(unsigned int * cache,unsigned int * memory);
int setupL1();
int setupL2();
ref bundleRef(char op, unsigned long long int address, unsigned int bytesize);
int fillBuffer();


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

    computeCost(cconfig,mconfig);

    setupL1();

    int count = fillBuffer();

    printf("Read in %d elements from the stream.\n",count);

    for(int i = 0; i < count; i++) {
        printf("Data!:\n");
        printf("Op: %c, address: %Lx, bytesize: %d\n",inbuffer[i].op, inbuffer[i].address, inbuffer[i].bytesize);
    }

    printf("main waiting for input buffer thread to terminate...\n");

    // for(int k = 0; k<100;k++) {
    //     printf("Should be zero: %d.\n", l1cache[k].valid);
    // }

	

	return 0;
}

int fillBuffer() {
        int fill = 0;
    while(scanf("%c %Lx %d\n",&op,&address,&bytesize)==3) {
        inbuffer[fill] = bundleRef(op, address, bytesize);
        fill++;
    }
    return fill;
}

ref bundleRef(char op, unsigned long long int address, unsigned int bytesize) {
    ref newRef;
    newRef.op = op;
    newRef.address = address;
    newRef.bytesize = bytesize;
    return newRef;
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
    printf("Here is the passed in cache parameter values:\n");
    for(int q = 0; q < 12; q++) {
    	printf("%d: %d\n", q, cconfig[q]);
    }


    int mnum = 0;
    for(int i = 0; i < 6; i++) {
    	fgets(mbuf[i],20,mc);
    	int w = 0;
    	// while(mbuf[i][w] != 0) {
    	// 	printf("%c",mbuf[i][w]);
    	// 	w++;
    	// }
    	const char * mstr = strstr(mbuf[i],":");

    	if(mstr) {
    		printf("\n");
    		int w = 0;
    		int result = atoi(mstr+1);
    		// printf("The integer result is: %d.\n",result);
    		mconfig[mnum] = result;
    		mnum++;
    	}
    }
    printf("\n\n\n");
    printf("Here is the passed in memory parameter values:\n");
    for(int q = 0; q < 4; q++) {
    	printf("%d: %d\n", q, mconfig[q]);
    }
               
	fclose(cc);
	fclose(mc);

	return 0;

}

int computeCost(unsigned int cache[],unsigned int * memory) {
    int cost = 0;
    int l1assoc = 0;
    int l1size = 0;
    int l1mult = 1;
    int l2assoc = 0;
    int l2size = 0;
    int l2mult = 1;
    int mem_ready = 0;
    int mem_chunksize = 0;

    l1size = (cache[1]/1024)/4; //Number of sets of 4KB
    l1assoc = cache[2];
    printf("L1 Cache Size(Multiples of 4KB): %d\n",l1size);
    printf("L1 Cache Associativity: %d\n",l1assoc);

    l2size = (cache[6]/1024)/16; //Number of set of 16KB
    l2assoc = cache[7];
    printf("L2 Cache Size(Multiples of 16KB): %d\n",l2size);
    printf("L2 Cache Associativity: %d\n",l2assoc);

    mem_ready = 50/ memory[1];
    printf("Memory Latency Increased by factor of %d.\n",mem_ready);

    mem_chunksize = memory[3] / 8;
    printf("Memory Chunksize multiples of 8 Bytes: %d.\n",mem_chunksize);

    //Full cost calculation.

    cost = 100*l1size;
    while(l1assoc > 1) {
        l1assoc >>= 1;
        l1mult++;
    }
    printf("The Associativity multiplier for L1 is: %d.\n",l1mult);

    while(l2assoc > 1) {
        l2assoc >>= 1;
        l2mult++;
    }
    printf("The Associativity multiplier for L2 is: %d.\n",l2mult);

    cost = (100*l1size) + ((l1mult-1)*l1size*100);
    cost += (50*l2size) + ((l2mult-1)*l2size*50);

    printf("Cost for just L1 and L2 size and Associativity: %d\n",cost);

    cost += (50*mem_ready);
    cost += (25*mem_chunksize);

    printf("Total Cost of Memory System: %d.\n",cost);


    return cost;
}

int setupL1() {
    int l1blocks = cconfig[1]/cconfig[0]; //cache size / block size
    int l1assoc = cconfig[2];
    l1blocks /= l1assoc; //l1blocks = l1block/associativity
    l1cache = malloc(sizeof(c_entry)*l1blocks*l1assoc);
    if(l1cache) printf("Successfully allocated memory!\n");

    for(int j = 0; j < (l1blocks*l1assoc); j++) {
        // l1cache[j] = (c_entry) malloc(sizeof(c_entry));
        l1cache[j].index = 0;
        l1cache[j].valid = 0;
        l1cache[j].tag = 0;
        l1cache[j].data = 0;
    }
    return 0;
}

    // for(int set = 0; set < l1assoc; set++) {
    //     for(int block = 0; block < l1blocks; block++) {
    //         int offset = block*(set+1);
    //         l1assoc[offset].index = 0;
    //         l1assoc[offset].valid = 0;
    //         l1assoc[offset].tag = 0;
    //         l1assoc[offset].data = 0;
    //     }
    // }