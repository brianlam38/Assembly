// vmsim.c ... Virtual memory simulation
// COMP1521 17s2 Assignment 2
// Written by John Shepherd, September 2017
//
// Sets up memory, page table, replacement policy, etc.
// Reads sequence of memory references to simulate program execution
// Prints running statistics and summary statistics after execution ends

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "Memory.h"
#include "PageTable.h"
#include "Stats.h"

int processArgs(int, char **, int *, int *, int *);
int getNextReference(int *, char *);

// main: setup and run the simulation

int main(int argc, char *argv[])
{
   int policy;  // page replacement policy
   int nFrames; // size of physical memory (in # frames)
   int nPages;  // size of virtual memory / process address space [ pages / frames are the same size ]
                //   -> when you want to ref a page in a process addr space, it needs to be loaded into
                //      a frame in virtual memory

   // grab values off cmd line
   // uses these values to initialise data structures + stat counters
   // if debugging, it will give a dump of the initial stage of the page table
   if (!processArgs(argc, argv, &policy, &nPages, &nFrames))
      return EXIT_FAILURE;

   // Set up data structures
   initMemory(nFrames);
   initPageTable(policy,nPages);
   initStats();

#ifdef DBUG
   showPageTableStatus();
#endif

   int  pageNo;    // which page is accessed
   char mode;      // accessed for read or write
   int  time = 0;  // current time on clock

   // Simulator reads a sequence of memory references (read / write + which page to read / write from)
   // When references run out, it means the program has finished executing
   // These references are supposed to be a TRACE of memory access behaviour of a program.
   //    e.g. starts reading from pg 0, writing to pg 7, reading pg 1, writing pg 6 etc.
   while (getNextReference(&pageNo, &mode)) {
#ifdef DBUG
      printf("\n=== Request %c%d @ t=%d ===\n",mode,pageNo,time);
#endif
      // read request - look at something inside a page
      if (mode == 'r')
         countPeekRequest();
      // write request - writing / modifying existing page
      else
         countPokeRequest();
      // IMPORT
      // Given a memory addr, convert it into an offset / pageNo and make sure relevant page is loaded into
      // memory once we know where in memory we need to load it. We cna an absolute memory address that we can use.
      // MODE / TIME are purely for statistics tracking
      //   -> Mode: Read / write
      //   -> Time: A clock-tick, starting at 0 and tick for every request. Every request happens at a new time.
      requestPage(pageNo, mode, time);
      time++;
// Debugging after we process the request
#ifdef DBUG
      showPageTableStatus();
#endif
   }

   showSummaryStats();

   return EXIT_SUCCESS;
}

// processArgs: process command-line arguments

int processArgs(int argc, char **argv, int *pol, int *np, int *nf)
{
   // Process command-line args
   if (argc < 4) {
      fprintf(stderr, "Usage: %s Policy #Pages #Frames\n", argv[0]);
      return 0;
   }
   if (strcasecmp(argv[1],"LRU") == 0)
      *pol = REPL_LRU;
   else if (strcasecmp(argv[1],"FIFO") == 0)
      *pol = REPL_FIFO;
   else if (strcasecmp(argv[1],"Clock") == 0)
      *pol = REPL_CLOCK;
   else {
      fprintf(stderr, "%s: invalid Policy %s\n", argv[0], argv[1]);
      fprintf(stderr, "Policy must be one of LRU|FIFO|Clock\n");
      return 0;
   }
   if (sscanf(argv[2], "%d", np) < 1) {
      fprintf(stderr, "%s: invalid #Pages %s\n", argv[0], argv[2]);
      return 0;
   }
   if (*np < MIN_PAGES) {
      fprintf(stderr, "%s: #Pages must be >= %d\n", argv[0], MIN_PAGES);
      return 0;
   }
   if (sscanf(argv[3], "%d", nf) < 1) {
      fprintf(stderr, "%s: invalid #Frames\n", argv[0]);
      return 0;
   }
   if (*nf < MIN_FRAMES) {
      fprintf(stderr, "%s: #Frames must be >= %d\n", argv[0], MIN_FRAMES);
      return 0;
   }
   return 1;
}

// getNextReference: get next page reference from stdin

int getNextReference(int *page, char *mode)
{
   int ch;
   // skip white space
   ch = getchar();
   while (ch != EOF && isspace(ch))
      ch = getchar();
   if (ch == 'r' || ch == 'w')
      *mode = ch;
   else
      return 0;
   if (scanf("%d", page) == 1)
      return 1;
   else
      return 0;
}
