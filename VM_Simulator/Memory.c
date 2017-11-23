// Memory.c ... implementation of Memory operations
// COMP1521 17s2 Assignment 2
// Written by John Shepherd, September 2017

#include <stdlib.h>
#include <stdio.h>
#include "Memory.h"
#include "Stats.h"

// Symbolic constants

#define NO_NEXT -1
#define NEVER_LOADED -1

// MFI = Memory Frame Info

typedef struct {
   int whichPage;   // page from process address space
   int whenLoaded;  // when loaded, in clock ticks
} MFI;

// The physical memory is represented by an array
//  of Memory Frame Info structs, indexed by frameNo
// The Memory object is not directly accessible
//  outside this file (hence the static declaration)

static MFI *Memory;     // Single memory object, a representation of physical memory
static int  nFrames;    // Each memory frame is rep by minimal amount of inf
                        // (In reality memory frame wouldn't contain info related to page table, simply
                        //   the data for the page that has been loaded.)

// initMemory: create Memory data structure

void initMemory(int nf)
{  
   // mallocing Memory object
   Memory = malloc(nf * sizeof(MFI));
   if (Memory == NULL) {
      fprintf(stderr, "Can't initialise Memory\n");
      exit(EXIT_FAILURE);
   }
   // initialising memory frames + flags
   nFrames = nf;
   for (int i = 0; i < nFrames; i++) {
      Memory[i].whichPage = NONE;
      Memory[i].whenLoaded = NEVER_LOADED;
   }
}

// saveFrame: copy a frame to disk before replacing
// In this simulation, all we need to do is count the write

void saveFrame(int fno)
{
   countPageSave();
}

// findFreeFrame: scan Memory looking for unused slot
// returns NO_PAGE if no free slots, otherwise returns frame index

int findFreeFrame()
{
   for (int i = 0; i < nFrames; i++) {
      if (Memory[i].whichPage == NONE) return i;
   }
   return NONE;
}

// loadFrame: load a frame with a particular page

void loadFrame(int fno, int pno, int when)
{
   Memory[fno].whichPage = pno;
   Memory[fno].whenLoaded = when;
   countPageLoad();
}
