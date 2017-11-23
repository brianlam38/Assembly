// Stats.c ... implementation of VM stats
// COMP1521 17s2 Assignment 2
// Written by John Shepherd, September 2017

#include <stdlib.h>
#include <stdio.h>
#include "Stats.h"

// Statistics accumulators
// These variables are not visible outside this file

static int nPeekRequests;  // # requests to read from a page
static int nPokeRequests;  // # requests to change a page
static int nLoads;         // # times a page is loaded into memory	-> Load into disk
static int nSaves;         // # times a page is saved to disk		-> Write to disk
static int nHits;          // # requests answered by cached page
static int nFaults;        // # requests requiring page load      -> Page fault

// Statistics operations

void initStats()
{
   nPeekRequests = nPokeRequests = 0;
   nLoads = nSaves = nHits = nFaults = 0;
}

// Initialise counter functions
// Functions will simply increment counters + summary stats at the end
void countPeekRequest() { nPeekRequests++; }
void countPokeRequest() { nPokeRequests++; }
void countPageLoad() { nLoads++; }
void countPageSave() { nSaves++; }
void countPageFault() { nFaults++; }
void countPageHit() { nHits++; }

void showSummaryStats()
{
   int nRequests = nPeekRequests + nPokeRequests;
   printf("\n=====Summary ====\n");
   printf("Requests:  #peeks: %d,  #pokes: %d,  total: %d\n",
           nPeekRequests, nPokeRequests, nRequests);
   printf("Paging:    #hits: %d,  #faults: %d,  #loads: %d,  #saves: %d\n",
           nHits, nFaults, nLoads, nSaves);
}
