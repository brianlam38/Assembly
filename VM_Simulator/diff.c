1c1
< // vmsim.c ... Virtual memory simulation
---
> // PageTable.c ... implementation of Page Table operations
4,7d3
< //
< // Sets up memory, page table, replacement policy, etc.
< // Reads sequence of memory references to simulate program execution
< // Prints running statistics and summary statistics after execution ends
11,13d6
< #include <string.h>
< #include <strings.h>
< #include <ctype.h>
15d7
< #include "PageTable.h"
16a9,39
> #include "PageTable.h"
> 
> // Symbolic constants
> 
> #define NOT_USED 0
> #define IN_MEMORY 1
> #define ON_DISK 2
> 
> // PTE = Page Table Entry
> 
> typedef struct {
>    char status;      // NOT_USED, IN_MEMORY, ON_DISK
>    char modified;    // boolean: changed since loaded
>    int  frame;       // memory frame holding this page
>    int  accessTime;  // clock tick for last access
>    int  loadTime;    // clock tick for last time loaded
>    int  nPeeks;      // total number times this page read
>    int  nPokes;      // total number times this page modified
>    // TODO: add more fields here, if needed ...
> } PTE;
> 
> // The virtual address space of the process is managed
> //  by an array of Page Table Entries (PTEs)
> // The Page Table is not directly accessible outside
> //  this file (hence the static declaration)
> 
> static PTE *PageTable;      // array of page table entries
> static int  nPages;         // # entries in page table
> static int  replacePolicy;  // how to do page replacement
> static int  fifoList;       // index of first PTE in FIFO list
> static int  fifoLast;       // index of last PTE in FIFO list
18,19c41
< int processArgs(int, char **, int *, int *, int *);
< int getNextReference(int *, char *);
---
> // Forward refs for private functions
21c43
< // main: setup and run the simulation
---
> static int findVictim(int);
23c45,47
< int main(int argc, char *argv[])
---
> // initPageTable: create/initialise Page Table data structures
> 
> void initPageTable(int policy, int np)
25,40c49,67
<    int policy;  // page replacement policy
<    int nFrames; // size of physical memory (in # frames)
<    int nPages;  // size of virtual memory / process address space [ pages / frames are the same size ]
<                 //   -> when you want to ref a page in a process addr space, it needs to be loaded into
<                 //      a frame in virtual memory
< 
<    // grab values off cmd line
<    // uses these values to initialise data structures + stat counters
<    // if debugging, it will give a dump of the initial stage of the page table
<    if (!processArgs(argc, argv, &policy, &nPages, &nFrames))
<       return EXIT_FAILURE;
< 
<    // Set up data structures
<    initMemory(nFrames);
<    initPageTable(policy,nPages);
<    initStats();
---
>    PageTable = malloc(np * sizeof(PTE));
>    if (PageTable == NULL) {
>       fprintf(stderr, "Can't initialise Memory\n");
>       exit(EXIT_FAILURE);
>    }
>    replacePolicy = policy;
>    nPages = np;
>    fifoList = 0;
>    fifoLast = nPages-1;
>    for (int i = 0; i < nPages; i++) {
>       PTE *p = &PageTable[i];
>       p->status = NOT_USED;
>       p->modified = 0;
>       p->frame = NONE;
>       p->accessTime = NONE;
>       p->loadTime = NONE;
>       p->nPeeks = p->nPokes = 0;
>    }
> }
42,44c69,72
< #ifdef DBUG
<    showPageTableStatus();
< #endif
---
> // requestPage: request access to page pno in mode
> // returns memory frame holding this page
> // page may have to be loaded
> // PTE(status,modified,frame,accessTime,nextPage,nPeeks,nWrites)
46,72c74,88
<    int  pageNo;    // which page is accessed
<    char mode;      // accessed for read or write
<    int  time = 0;  // current time on clock
< 
<    // Simulator reads a sequence of memory references (read / write + which page to read / write from)
<    // When references run out, it means the program has finished executing
<    // These references are supposed to be a TRACE of memory access behaviour of a program.
<    //    e.g. starts reading from pg 0, writing to pg 7, reading pg 1, writing pg 6 etc.
<    while (getNextReference(&pageNo, &mode)) {
< #ifdef DBUG
<       printf("\n=== Request %c%d @ t=%d ===\n",mode,pageNo,time);
< #endif
<       // read request - look at something inside a page
<       if (mode == 'r')
<          countPeekRequest();
<       // write request - writing / modifying existing page
<       else
<          countPokeRequest();
<       // IMPORT
<       // Given a memory addr, convert it into an offset / pageNo and make sure relevant page is loaded into
<       // memory once we know where in memory we need to load it. We cna an absolute memory address that we can use.
<       // MODE / TIME are purely for statistics tracking
<       //   -> Mode: Read / write
<       //   -> Time: A clock-tick, starting at 0 and tick for every request. Every request happens at a new time.
<       requestPage(pageNo, mode, time);
<       time++;
< // Debugging after we process the request
---
> int requestPage(int pno, char mode, int time)
> {
>    if (pno < 0 || pno >= nPages) {
>       fprintf(stderr,"Invalid page reference\n");
>       exit(EXIT_FAILURE);
>    }
>    PTE *p = &PageTable[pno];
>    int fno; // frame number
>    switch (p->status) {
>    case NOT_USED:
>    case ON_DISK:
>       // TODO: add stats collection
>       fno = findFreeFrame();
>       if (fno == NONE) {
>          int vno = findVictim(time);
74c90
<       showPageTableStatus();
---
>          printf("Evict page %d\n",vno);
75a92,121
>          // TODO:
>          // if victim page modified, save its frame
>          // collect frame# (fno) for victim page
>          // update PTE for victim page
>          // - new status
>          // - no longer modified
>          // - no frame mapping
>          // - not accessed, not loaded
>       }
>       printf("Page %d given frame %d\n",pno,fno);
>       // TODO:
>       // load page pno into frame fno
>       // update PTE for page
>       // - new status
>       // - not yet modified
>       // - associated with frame fno
>       // - just loaded
>       break;
>    case IN_MEMORY:
>       // TODO: add stats collection
>       break;
>    default:
>       fprintf(stderr,"Invalid page status\n");
>       exit(EXIT_FAILURE);
>    }
>    if (mode == 'r')
>       p->nPeeks++;
>    else if (mode == 'w') {
>       p->nPokes++;
>       p->modified = 1;
77,80c123,124
< 
<    showSummaryStats();
< 
<    return EXIT_SUCCESS;
---
>    p->accessTime = time;
>    return p->frame;
83c127,128
< // processArgs: process command-line arguments
---
> // findVictim: find a page to be replaced
> // uses the configured replacement policy
85c130
< int processArgs(int argc, char **argv, int *pol, int *np, int *nf)
---
> static int findVictim(int time)
87,89c132,140
<    // Process command-line args
<    if (argc < 4) {
<       fprintf(stderr, "Usage: %s Policy #Pages #Frames\n", argv[0]);
---
>    int victim = 0;
>    switch (replacePolicy) {
>    case REPL_LRU:
>       // TODO: implement LRU strategy
>       break;
>    case REPL_FIFO:
>       // TODO: implement FIFO strategy
>       break;
>    case REPL_CLOCK:
92,119c143
<    if (strcasecmp(argv[1],"LRU") == 0)
<       *pol = REPL_LRU;
<    else if (strcasecmp(argv[1],"FIFO") == 0)
<       *pol = REPL_FIFO;
<    else if (strcasecmp(argv[1],"Clock") == 0)
<       *pol = REPL_CLOCK;
<    else {
<       fprintf(stderr, "%s: invalid Policy %s\n", argv[0], argv[1]);
<       fprintf(stderr, "Policy must be one of LRU|FIFO|Clock\n");
<       return 0;
<    }
<    if (sscanf(argv[2], "%d", np) < 1) {
<       fprintf(stderr, "%s: invalid #Pages %s\n", argv[0], argv[2]);
<       return 0;
<    }
<    if (*np < MIN_PAGES) {
<       fprintf(stderr, "%s: #Pages must be >= %d\n", argv[0], MIN_PAGES);
<       return 0;
<    }
<    if (sscanf(argv[3], "%d", nf) < 1) {
<       fprintf(stderr, "%s: invalid #Frames\n", argv[0]);
<       return 0;
<    }
<    if (*nf < MIN_FRAMES) {
<       fprintf(stderr, "%s: #Frames must be >= %d\n", argv[0], MIN_FRAMES);
<       return 0;
<    }
<    return 1;
---
>    return victim;
122c146,147
< // getNextReference: get next page reference from stdin
---
> // showPageTableStatus: dump page table
> // PTE(status,modified,frame,accessTime,nextPage,nPeeks,nWrites)
124c149
< int getNextReference(int *page, char *mode)
---
> void showPageTableStatus(void)
126,138c151,179
<    int ch;
<    // skip white space
<    ch = getchar();
<    while (ch != EOF && isspace(ch))
<       ch = getchar();
<    if (ch == 'r' || ch == 'w')
<       *mode = ch;
<    else
<       return 0;
<    if (scanf("%d", page) == 1)
<       return 1;
<    else
<       return 0;
---
>    char *s;
>    printf("%4s %6s %4s %6s %7s %7s %7s %7s\n",
>           "Page","Status","Mod?","Frame","Acc(t)","Load(t)","#Peeks","#Pokes");
>    for (int i = 0; i < nPages; i++) {
>       PTE *p = &PageTable[i];
>       printf("[%02d]", i);
>       switch (p->status) {
>       case NOT_USED:  s = "-"; break;
>       case IN_MEMORY: s = "mem"; break;
>       case ON_DISK:   s = "disk"; break;
>       }
>       printf(" %6s", s);
>       printf(" %4s", p->modified ? "yes" : "no");
>       if (p->frame == NONE)
>          printf(" %6s", "-");
>       else
>          printf(" %6d", p->frame);
>       if (p->accessTime == NONE)
>          printf(" %7s", "-");
>       else
>          printf(" %7d", p->accessTime);
>       if (p->loadTime == NONE)
>          printf(" %7s", "-");
>       else
>          printf(" %7d", p->loadTime);
>       printf(" %7d", p->nPeeks);
>       printf(" %7d", p->nPokes);
>       printf("\n");
>    }
