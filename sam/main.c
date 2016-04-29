#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "main.h"
// #include <cstdlib.h>


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

    setupL1Dcache();
    setupL1Icache();

    int count = startSim();

    // for(int i = 0; i < count; i++) {
    //     printf("Data!:\n");
    //     printf("Op: %c, address: %Lx, bytesize: %d\n",inbuffer[i].op, inbuffer[i].address, inbuffer[i].bytesize);
    // }

    printL1('I');
    printL1('D');

    printf("FINISHED...\n");

    free(l1Icache);
    free(l1Dcache);
	return 0;
}

int startSim() {
    ref entry;
    while(scanf("%c %Lx %d\n",&op,&address,&bytesize)==3) {
        total++;
        entry = bundleRef(op, address, bytesize);
    }
    return 0;
}

ref bundleRef(char op, unsigned long long int address, unsigned int bytesize) {
    ref newRef;
    newRef.op = op;
    newRef.address = address;
    newRef.bytesize = bytesize;
    return newRef;
}

int processRef(ref newref) {
    static int firstrun = 1;

    int hit = 0; //Status variable for if the current instruction is a hit or not.

    if(firstrun) //Function to set up the masks, but not make it do it every time. Hopefully speed improvement.
    {
        firstrun = 0;
        for(int i = 0; i < addrbits; i++) {
            blockmask <<= 1;
            blockmask++;
        }
    }


    uint32_t blockaddr = 0;
    uint32_t tag = 0;

    int iod = -1; //iod = instruction or data


    printf("The blockmask! %#15x\n",blockmask);



    //Count the number of instructions of each type
        blockaddr = (newref.address & blockmask) % l1blocks;
        printf("This is the address!!! %Lx \n",newref.address);

        tag = ( newref.address & (~blockmask) ) >> (32-addrbits);  

        if(newref.op == 'R')
        {
            printf("Read Memory!\n");
            tread++;
            iod = 1;
        }
        if(newref.op == 'W')
        {
            printf("Write Memory!\n");
            twrite++;
            iod = 1;
        }
        if(newref.op == 'I')
        {
            printf("Read Instruction!\n");
            tinst++;
            iod = 0;
        }
        printf("Block address for trace %d is: %#15x.\n",total,blockaddr);
        printf("Tag for trace %d is: %#15x.\n",total,tag);


        //Check if the instruction is in the L1 cache:
        //Map the block to the array
        c_entry * currentl1;
        if(iod) currentl1 = l1Dcache;
        else currentl1 = l1Icache;
        //Case check if item is currently in cache.
        for(int i = 0; i< l1assoc; i++) {
            if(currentl1[(blockaddr*l1assoc+i)].valid 
                && currentl1[(blockaddr*l1assoc+i)].tag == tag) { //Check to see if any one of the available slots are not valid
                printf("Cache hit!!\n");
                hit = 1;
                break;
            }
        }
        if(!hit) { //cache miss, deal with it
            missHandlerL1(currentl1,tag,blockaddr);
        }

        return hit;

    }


    //MOVE THIS
    // printf("Reads: %d, Writes: %d, Instructions: %d.\n",read,write,inst);
    // printout("-----------------------------");
    // printout("Instruction Statistics");
    // printout("-----------------------------");
    // printout("Number of Reference Types: [Percentage]");
    // sprintf(printbuffer,"Writes:          %d  [%.3f]",write,(float)write/total);
    // printout(printbuffer);
    // sprintf(printbuffer,"Reads:           %d  [%.3f]",read,(float)read/total);
    // printout(printbuffer);
    // sprintf(printbuffer,"Instructions:    %d  [%.3f]",inst,(float)inst/total);
    // printout(printbuffer);
    // printout("-----------------------------");
    // sprintf(printbuffer,"Total:           %d ",total);
    // printout(printbuffer);



void missHandlerL1(c_entry * cache, uint32_t tag, uint32_t blockaddr) {
    //LRU Setup
    for(int i = 0; i< l1assoc; i++)  {
        if(cache[(blockaddr*l1assoc+i)].valid)
            cache[(blockaddr*l1assoc+i)].lru++; //Increment LRU variable so oldest one has highest value 
    }
    //Check for available slots
    //Case there is a slot available
    for(int i = 0; i< l1assoc; i++) {
        if(!cache[(blockaddr*l1assoc+i)].valid) { //Check to see if any one of the available slots are not valid
            cache[(blockaddr*l1assoc+i)].valid = 1;
            cache[(blockaddr*l1assoc+i)].tag = tag;
            cache[(blockaddr*l1assoc+i)].lru = 0;
            printf("Available Slot to address miss!!\n");
            return;
        }
    }  
    //Case evict oldest cache entry
    int oldlru = 0;
    int oldlruindex = 0;
    //Determine oldest element
    for(int i = 0; i< l1assoc; i++) {
        if(cache[(blockaddr*l1assoc+i)].valid = 1) {
            if(oldlru < cache[(blockaddr*l1assoc+i)].lru) {
                oldlruindex = i;
                oldlru = cache[(blockaddr*l1assoc+i)].lru;
            }
        }
    }
    //Push the older entry in an 8-entry LRU stack
    printf("Evicted entry from cache!\n");
    //Write entry into old 
    cache[(blockaddr*l1assoc+oldlruindex)].tag = tag; //Update cache entry
    return;


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



int setupL1Dcache() {
    l1blocks = cconfig[1]/cconfig[0]; //cache size / block size
    l1assoc = cconfig[2];
    l1blocks /= l1assoc; //l1blocks = l1block/associativity
    l1Dcache = malloc(sizeof(c_entry)*l1blocks*l1assoc);
    if(l1Icache) printf("Successfully allocated memory!\n");

    int bits = 0;
    while((cconfig[1] >> bits) != 1) bits++; //Determine the number of bits to use for block address
    addrbits = bits;


    printf("The number of bits needed for the block address is: %d.\n",bits);



    for(int j = 0; j < ((l1blocks*l1assoc)-1); j++) {
        l1Dcache[j].valid = 0;
        l1Dcache[j].tag = 0;
        l1Dcache[j].dirty = 0;
    }
    return 0;
}

int setupL1Icache() {
    l1blocks = cconfig[1]/cconfig[0]; //cache size / block size
    l1assoc = cconfig[2];
    l1blocks /= l1assoc; //l1blocks = l1block/associativity
    l1Icache = malloc(sizeof(c_entry)*l1blocks*l1assoc);
    if(l1Icache) printf("Successfully allocated memory!\n");

    int bits = 0;
    while((cconfig[1] >> bits) != 1) bits++; //Determine the number of bits to use for block address
    addrbits = bits;


    printf("The number of bits needed for the block address is: %d.\n",bits);



    for(int j = 0; j < (l1blocks*l1assoc); j++) {
        l1Icache[j].valid = 0;
        l1Icache[j].tag = 0;
        l1Icache[j].dirty = 0;
        printf("Loop number %d\n",j);
    }
    return 0;
}

void printL1(char t) {
    int valid = 0;
    if(t=='I') {
        printf("L1 Instruction Cache:\n");
        for(int k = 0; k < l1assoc; k++) {
            printf("Set %d ----------------------\n",k);
            for(int i = 0; i < l1blocks; i++) {
                if(l1Icache[(i*l1assoc)+k].valid) valid++;
                printf("Block Addr: %x, Tag: %x, Valid: %x.\n",i,l1Icache[(i*l1assoc)+k].tag,l1Icache[(i*l1assoc)+k].valid);
            }
        }
    }
    if(t=='D') {
        printf("L1 Data Cache:\n");
        for(int k = 0; k < l1assoc; k++) {
            printf("Set %d ----------------------\n",k);
            for(int i = 0; i < l1blocks; i++) {
                if(l1Dcache[(i*l1assoc)+k].valid) valid++;
                printf("Block Addr: %x, Tag: %x, Valid: %x.\n",i,l1Dcache[(i*l1assoc)+k].tag,l1Dcache[(i*l1assoc)+k].valid);
            }
        }
    }
    printf("Number of valid entries: %d.\n",valid);
}


int printout(const char * outstring) {
    FILE *out = fopen("printout.txt","a+");
    int chars = fprintf(out, "%s\n",outstring);
    fclose(out);   

    return chars;
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