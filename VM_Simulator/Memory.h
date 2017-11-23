// Memory.h ... interface to Memory operations
// COMP1521 17s2 Assignment 2
// Written by John Shepherd, September 2017

#ifndef MEMORY_H

// How many frames in the main memory?
// Don't allow less than four
// May be smaller than process address space

#define MIN_FRAMES 4

// Value to indicate non-existent page/frame

#define NONE -1

// Operations on Memory

void initMemory(int nframes);
int  findFreeFrame();
void saveFrame(int fno);
void loadFrame(int fno, int pno, int when);

#endif
