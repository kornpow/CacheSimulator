#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
// #include <cstdlib.h>

#define BSIZE 100

unsigned int cconfig[17];
unsigned int mconfig[4];

char op;
unsigned long long int address;
unsigned int bytesize;


int l1blocks = 0;
int l2blocks = 0;
int addrbits = 0;

char printbuffer[50];

// ***** Structure Declaration *****

typedef struct c_entry{
    uint32_t index;
    uint8_t valid;
    uint8_t tag;
    long long int data;
    uint8_t dirty;
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
int processBuffer(int size);
int printout(const char * outstring);


int main(int argc, char ** argv) {
	printf("Number of parameters: %d\n",argc);
	if(argc != 2)
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
    int processed = processBuffer(count);

    printf("Read in %d elements from the stream.\n",count);

    for(int i = 0; i < count; i++) {
        printf("Data!:\n");
        printf("Op: %c, address: %Lx, bytesize: %d\n",inbuffer[i].op, inbuffer[i].address, inbuffer[i].bytesize);
    }



    // int chars = writeOutputFile("testing 123");


    // printf("FINISHED...\n");
	return 0;
}

int printout(const char * outstring) {
    FILE *out = fopen("printout.txt","a+");
    int chars = fprintf(out, "%s\n",outstring);
    fclose(out);   

    return chars;
 }

int processBuffer(int size) {
    int processed = 0;
    int item;
    int read = 0;
    int write = 0;
    int inst = 0;

    int blockaddr = 0;
    int tag = 0;

    int blockmask = 0;

    for(int i = 0; i < addrbits; i++) {
        blockmask <<= 1;
        blockmask++;
    }

    printf("The blockmask! %#15x",blockmask);



    //Count the number of instructions of each type
    for(int i = 0; i < size; i++) {
        blockaddr = (inbuffer[i].address & blockmask) % l1blocks;
        tag = (inbuffer[i].address & (!blockmask) );  

        if(inbuffer[i].op == 'R')
        {
            printf("Read Memory!\n");
            read++;
        }
        if(inbuffer[i].op == 'W')
        {
            printf("Write Memory!\n");
            write++;
        }
        if(inbuffer[i].op == 'I')
        {
            printf("Read Instruction!\n");
            inst++;
        }
        printf("Block address for trace %d is: %#15x.\n",i,blockaddr);
        printf("Tag for trace %d is: %#15x.\n",i,tag);


    }
    int total = read + write + inst;
    printf("Reads: %d, Writes: %d, Instructions: %d.\n",read,write,inst);
    printout("-----------------------------");
    printout("Instruction Statistics");
    printout("-----------------------------");
    printout("Number of Reference Types: [Percentage]");
    sprintf(printbuffer,"Writes:          %d  [%.3f]",write,(float)write/total);
    printout(printbuffer);
    sprintf(printbuffer,"Reads:           %d  [%.3f]",read,(float)read/total);
    printout(printbuffer);
    sprintf(printbuffer,"Instructions:    %d  [%.3f]",inst,(float)inst/total);
    printout(printbuffer);
    printout("-----------------------------");
    sprintf(printbuffer,"Total:           %d ",total);
    printout(printbuffer);

    return processed;
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
	printf("Usage: zcat <trace> | <progname> <cache_config>\n");
}

int readConfig(char *cache, char *mem) {
	FILE *cc = fopen(cache,"r");
	if ( cc == 0 )
	{
		printf( "Could not open cache file\n" );
		return -1;
	}

	int x;
	char cbuf[25][21];
	char buf[21] = {0};

	//Get L1
	// fgets(buf,20,cc);
	// for(int j = 0; j <20; j++) {
	// 	printf("%c",buf[j]);
	// }


    int cnum = 0;
    for(int i = 0; i < 25; i++) {
    	fgets(cbuf[i],21,cc);
    	int w = 0;
    	while(cbuf[i][w] != 0) {
    		printf("%c",cbuf[i][w]);
    		w++;
    	}
    	const char * cstr = strstr(cbuf[i],":");
    	if(cstr) {
    		printf("\n");
    		int w = 0;
            if(i == 22) {
                printf("Trace Name: %s",cstr+1);
                printout("-----------------------------");
                printout(cstr+1);
                printout("-----------------------------");
            }
    		int result = atoi(cstr+1);
    		printf("The integer result is: %d.\n",result);
    		cconfig[cnum] = result;
    		cnum++;
    	}
    }
    printf("\n\n\n");
    printf("Here is the passed in cache parameter values:\n");
    for(int q = 0; q < 16; q++) {
    	printf("%d: %d\n", q, cconfig[q]);
    }   
    printout("-----------------------------");
    printout("Memory Configuration");
    printout("-----------------------------");
    printout(" ");
    sprintf(printbuffer,"Dcache size: %d, ways: %d, block size: %d",cconfig[1],cconfig[2],cconfig[0]);
    printout(printbuffer);
    sprintf(printbuffer,"Icache size: %d, ways: %d, block size: %d",cconfig[1],cconfig[2],cconfig[0]);
    printout(printbuffer);
    sprintf(printbuffer,"L2-cache size: %d, ways: %d, block size: %d",cconfig[6],cconfig[7],cconfig[5]);
    printout(printbuffer);
    sprintf(printbuffer,"Memory ready time: %d, chunksize: %d, chunktime: %d",cconfig[14],cconfig[15],cconfig[14]);
    printout(printbuffer);
    

	fclose(cc);

	return 0;

}

int computeCost(unsigned int cache[],unsigned int * memory) {
    printout(" ");
    printout("-----------------------------");
    printout("Memory Cost Calculations:");
    printout("-----------------------------");
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
    sprintf(printbuffer,"L1 Cache Size(Multiples of 4KB): %d\n",l1size);
    printout(printbuffer);

    printf("L1 Cache Associativity: %d\n",l1assoc);
    sprintf(printbuffer,"L1 Cache Associativity: %d\n",l1assoc);
    printout(printbuffer);

    l2size = (cache[6]/1024)/16; //Number of set of 16KB
    l2assoc = cache[7];

    printf("L2 Cache Size(Multiples of 16KB): %d\n",l2size);
    sprintf(printbuffer,"L2 Cache Size(Multiples of 16KB): %d\n",l2size);
    printout(printbuffer);

    printf("L2 Cache Associativity: %d\n",l2assoc);
    sprintf(printbuffer,"L2 Cache Associativity: %d\n",l2assoc);
    printout(printbuffer);


    mem_ready = 50/ cache[13];
    printf("Memory Latency Increased by factor of %d.\n",mem_ready);
    sprintf(printbuffer,"Memory Latency Increased by factor of %d.\n",mem_ready);
    printout(printbuffer);

    mem_chunksize = cache[15] / 8;
    printf("Memory Chunksize multiples of 8 Bytes: %d.\n",mem_chunksize);
    sprintf(printbuffer,"Memory Chunksize multiples of 8 Bytes: %d.\n",mem_chunksize);
    printout(printbuffer);

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
    sprintf(printbuffer,"Total Cost of Memory System: %d.\n",cost);
    printout(printbuffer);


    return cost;
}

int setupL1() {
    l1blocks = cconfig[1]/cconfig[0]; //cache size / block size
    int l1assoc = cconfig[2];
    l1blocks /= l1assoc; //l1blocks = l1block/associativity
    l1cache = malloc(sizeof(c_entry)*l1blocks*l1assoc);
    if(l1cache) printf("Successfully allocated memory!\n");

    int bits = 0;
    while((cconfig[1] >> bits) != 1) bits++; //Determine the number of bits to use for block address
    addrbits = bits;


    printf("The number of bits needed for the block address is: %d.\n",bits);



    for(int j = 0; j < (l1blocks*l1assoc); j++) {
        // l1cache[j] = (c_entry) malloc(sizeof(c_entry));
        l1cache[j].index = 0;
        l1cache[j].valid = 0;
        l1cache[j].tag = 0;
        l1cache[j].data = 0;
        l1cache[j].dirty = 0;
    }
    return 0;
}