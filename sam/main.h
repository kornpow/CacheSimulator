#define BSIZE 200

unsigned int cconfig[17];
unsigned int mconfig[4];

char op;
unsigned long long int address;
unsigned int bytesize;

//Variables to hold numbers of each type of instructions and total
int total = 0;
int tread = 0;
int twrite = 0;
int tinst = 0;

//Variables to hold calculated values about the cache, not from passed in parameters
//Assign these variable in a "first run" statement so they dont need to be calculated each iteration.
//Set in setupL1()
int l1blocks = 0;
int l2blocks = 0;
int l1assoc = 0;
//Set in processBuffer()
int addrbits = 0;
int blockmask = 0;

char printbuffer[50];

//Hit and miss variables
int hitcount = 0;
int misscount = 0;

// ***** Structure Declaration *****

typedef struct c_entry{
    uint32_t tag;
    uint8_t dirty;
    uint8_t valid;
    uint8_t lru;
} c_entry;

typedef struct {
    char op;
    unsigned long long int address;
    unsigned int bytesize;
} ref;

ref inbuffer[BSIZE]; //Buffer to hold cache trace entries.

// *****  END Structure Declaration *****
// *****  L1, L2, And LRU Cache Pointers
c_entry * l1Dcache;
c_entry * l1Icache;
c_entry * l2cache;

void usage();
int readConfig(char *cc, char *mc);
int computeCost(unsigned int * cache,unsigned int * memory);
int setupL1Dcache();
int setupL1Icache();
int setupL2();
ref bundleRef(char op, unsigned long long int address, unsigned int bytesize);
int startSim();
int processRef(ref newref);
int printout(const char * outstring);
void printL1(char t);
void missHandlerL1(c_entry * cache, uint32_t tag, uint32_t blockaddr);
