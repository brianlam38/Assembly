// PageTable.c ... implementation of Page Table operations
// COMP1521 17s2 Assignment 2
// Written by John Shepherd, September 2017
// Page Replacement Solution by Brian Lam z5035087

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "Memory.h"
#include "Stats.h"
#include "PageTable.h"

// Symbolic constants
#define NOT_USED 0
#define IN_MEMORY 1
#define ON_DISK 2

/* --- LRU / FIFO DATA STRUCTURES --- */

typedef struct _node* Node;
typedef struct _list* List;

typedef struct _node {
    int pno;
    Node next;
} node;

typedef struct _list {
    Node head;
    Node tail;
} list;

// global FIFO and LRU lists
static List FIFO;
static List LRU;

/* --- PAGE TABLE DATA STRUCTURES --- */

// Page Table Entry struct
typedef struct {
   char status;      // NOT_USED, IN_MEMORY, ON_DISK     in_mem = loaded | not_used / on_disk (if page contains initialised data) = not loaded
   char modified;    // written or not since load
   int  frame;       // frame holding this page
   int  accessTime;  // clock tick for last access (most recently read/written)
   int  loadTime;    // clock tick for last time loaded (last loaded)
   int  nPeeks;      // #times read (how many times did we look at the page)
   int  nPokes;      // #times modified (how many times did we write data into the page in memory)
   // add more if needed
} PTE;

static PTE *PageTable;      // array of page table entries (ptr to first entry)
static int  nPages;         // # entries in page table
static int  replacePolicy;  // how to do page replacement
static int findVictim(int); // private findVictim() function


/* --- LRU / FIFO LIST METHODS --- */

// create new list
List newList(void) {
   List l = malloc(sizeof(list));
   assert(l != NULL);
   l->head = NULL;
   l->tail = NULL;
   return l;
}

// free list
void freeList(List l) {
   Node curr = l->head;
   Node prev = NULL;
   while (curr != NULL) {
      prev = curr;
      curr = curr->next;
      free(prev);
   }
   free(l);
}
// Create new node + O(1) append to list tail
void append(List l, int pno) {
   Node new = malloc(sizeof(node));
   new->pno = pno;
   new->next = NULL;
   Node curr = l->tail;
   if (curr == NULL) {
      l->head = new;
      l->tail = new;
   } else {
      curr->next = new;
      l->tail = new;
   }
}

// Debugging: show pages in current list
void showPageList(List l) {
   printf("--- SHOWING PAGES IN LIST ---\n");
   Node curr = l->head;
   if (curr == NULL) {
      printf("NONE LOADED\n"); 
   }
   while (curr != NULL) {
      printf("[%d]-", curr->pno);
      curr = curr->next;
   }
}

// Place recently accessed page to tail of list
void updateLRUList(int page) {
   //printf("UPDATING PAGE = %d\n", page);
   //printf("LRU->head->pno = %d\n", LRU->head->pno);
   Node curr = LRU->head;
   Node prev = NULL;
   // find accessed page in LRU list
   while (curr != NULL) {
      //rintf("SCANNING THROUGH LIST\n");
      // scan list for page
      if (curr->pno == page) {
         //printf("1. CORRECT PAGE, BEGIN UPDATE\n");
         // if curr is head page
         if (curr == LRU->head) {
            //printf("LINK HEAD TO NEXT\n");
            LRU->head = curr->next;
            LRU->tail->next = curr;
            LRU->tail = curr;
            curr->next = NULL;
         } else {
            //printf("LINK PREV TO NEXT\n");
            prev->next = curr->next;
            LRU->tail->next = curr;
            LRU->tail = curr;
            curr->next = NULL;
         }
         //printf("4. DONE\n");
         break;         
      }
      //printf("NOT PAGE, CHECK NEXT");
      prev = curr;
      curr = curr->next;
   }
}

/* --- MAIN PAGE TABLE PROGRAM --- */

/* Initialise Page Table data */
void initPageTable(int policy, int np)
{
   // init page replacement data structures
   if (policy == REPL_FIFO) {
      printf("POLICY = FIFO\n");
      FIFO = newList();
   } else if (policy == REPL_LRU) {
      LRU = newList();
      printf("POLICY = LRU\n");
   }
   // init page table
   PageTable = malloc(np * sizeof(PTE));
   if (PageTable == NULL) {
      fprintf(stderr, "Can't initialise Memory\n");
      exit(EXIT_FAILURE);
   }
   // init global data
   replacePolicy = policy;
   nPages = np;
   // init all PTE array data
   for (int i = 0; i < nPages; i++) {
      PTE *p = &PageTable[i];
      p->status = NOT_USED;
      p->modified = 0;
      p->frame = NONE;
      p->accessTime = NONE;
      p->loadTime = NONE;
      p->nPeeks = p->nPokes = 0;
   }
}

/* 
 * Request access to page pno in mode
 * @return frame for this page
 */
int requestPage(int pno, char mode, int time)
{
#ifdef DBUG
   // show current list of pages
   if (replacePolicy == REPL_FIFO) {
      showPageList(FIFO);
   } else if (replacePolicy == REPL_LRU) {
      showPageList(LRU);
   }
#endif
   // check if pno is within valid range
   if (pno < 0 || pno >= nPages) {
      fprintf(stderr,"Invalid page reference\n");
      exit(EXIT_FAILURE);
   }
   // grab requested PTE
   PTE *p = &PageTable[pno];
   int fno;
   switch (p->status) {
      case NOT_USED:
      case ON_DISK:
         // pageFault++
         countPageFault();
         // free frame exists
         fno = findFreeFrame();
         // page replacement needed
         if (fno == NONE) {
            //printf(">> PAGE REPLACEMENT NEEDED\n");
            // grab victim pno
            int vno = findVictim(time);
   #ifdef DBUG
            printf("Evict page %d\n",vno);
   #endif
            // grab victim PTE
            PTE *v = &PageTable[vno];
            fno = v->frame;
            // if modified, save frame
            if (v->modified == 1) {
               saveFrame(fno);
               v->status = ON_DISK;
            }
            // update v.p status
            if (v->status != ON_DISK) {
               v->status = NOT_USED;
            }
            // reset modified, fno, access/load times    // DOES ACCESS/LOAD TIME RESET TO NONE AFTER FREEING?
            v->modified = 0;
            v->frame = NONE;
            v->accessTime = NONE;
            v->loadTime = NONE;
         } else {
            printf(">> FREE FRAME AVAILABLE\n");
         }
   #ifdef DBUG
         printf("Page %d given frame %d\n",pno,fno);
   #endif
         // load pno and time into frame
         int when = time;
         loadFrame(fno, pno, when);
         // update PTE
         p->status = IN_MEMORY;
         p->modified = 0;
         p->frame = fno;
         p->loadTime = when;
         // update FIFO or LRU list w/ new tail
         if (replacePolicy == REPL_FIFO) {
            append(FIFO, pno);
         } else if (replacePolicy == REPL_LRU) {
            append(LRU, pno);
            showPageList(LRU);
         }
         break;
      case IN_MEMORY:
         // PageHit++, request complete
         //printf(">> EXISTING FRAME\n");
         countPageHit();
         break;
   default:
      fprintf(stderr,"Invalid page status\n");
      exit(EXIT_FAILURE);
   }
   // READ - update peek, peekCtr++
   if (mode == 'r') {
      p->nPeeks++;
   // WRITE - update pokes + mod, pokeCtr++
   } else if (mode == 'w') {
      p->nPokes++;
      p->modified = 1;
   }
   // update access time
   p->accessTime = time;
   // update LRU list if policy = LRU && not just loaded
   // (if page was just loaded, it would be in correct order at the tail)
   if (replacePolicy == REPL_LRU && LRU->tail->pno != pno) {
      //printf("UPDATING LRU LIST\n\n");
      updateLRUList(pno);
   } else {
      //printf("JUST LOADED, LRU LIST UPDATE NOT NEEDED\n\n");
   }
   return p->frame;
}

/* 
 * Find a page to be replaced using selected repl policy
 * @return victim page no.
 */
static int findVictim(int time)
{
   int victim = 0;
   switch (replacePolicy) {
   case REPL_LRU:
      // TODO: implement LRU strategy
      // Keep track of all pages in a queue
      //    Most recently accessed page = tail of list.
      //    Least accessed page = head of list.
      // When replacement is needed, grab head of queue.

      // If page is re-loaded, update access time and move to tail
      victim = LRU->head->pno;
      // link to new head and rm old
      Node currLru = LRU->head;
      Node prevLru = currLru;
      currLru = currLru->next;
      LRU->head = currLru;
      free(prevLru);
      break;
   case REPL_FIFO:
      // TODO: implement FIFO strategy
      // Implement using a LIST (head and tail)
      // Keep track of all pages in a queue
      //    Most recent page = tail of list
      //    Oldest arrival in memory = head of list
      // When replacement is needed, grab head of the queue

      // O(1) operation to find victim pno
      victim = FIFO->head->pno;
      // link to new head and rm old
      Node curr = FIFO->head;
      Node prev = curr;
      curr = curr->next;
      FIFO->head = curr;
      free(prev);
      break;
   case REPL_CLOCK:
      return 0;
   }
   return victim;
}

/* Display Current PT Status */
void showPageTableStatus(void)
{
   char *s;
   printf("%4s %6s %4s %6s %7s %7s %7s %7s\n",
          "Page","Status","Mod?","Frame","Acc(t)","Load(t)","#Peeks","#Pokes");
   for (int i = 0; i < nPages; i++) {
      PTE *p = &PageTable[i];
      printf("[%02d]", i);
      switch (p->status) {
      case NOT_USED:  s = "-"; break;
      case IN_MEMORY: s = "mem"; break;
      case ON_DISK:   s = "disk"; break;
      }
      printf(" %6s", s);
      printf(" %4s", p->modified ? "yes" : "no");
      if (p->frame == NONE)
         printf(" %6s", "-");
      else
         printf(" %6d", p->frame);
      if (p->accessTime == NONE)
         printf(" %7s", "-");
      else
         printf(" %7d", p->accessTime);
      if (p->loadTime == NONE)
         printf(" %7s", "-");
      else
         printf(" %7d", p->loadTime);
      printf(" %7d", p->nPeeks);
      printf(" %7d", p->nPokes);
      printf("\n");
   }
}
