// PageTable.h ... interface to Page Table operations
// COMP1521 17s2 Assignment 2
// Written by John Shepherd, September 2017

#ifndef PAGETABLE_H

// Replacement policies

#define REPL_LRU 1
#define REPL_FIFO 2
#define REPL_CLOCK 3

// How many pages in the process address space?
// Don't allow less than two (main + stack)
// If references in input have pageNo > nPages
//    we generate an invalid address reference error

#define MIN_PAGES 2

// Operations on Page Table

void initPageTable(int policy, int nPages);
int requestPage(int pageNo, char mode, int time);
void showPageTableStatus(void);

#endif
