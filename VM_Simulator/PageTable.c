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

/* --- LRU & FIFO DATA STRUCTURES --- */

typedef struct _node* Node;
typedef struct _list* List;

typedef struct _node {
    int pno;
    Node next;
    Node prev;
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
   char status;      // NOT_USED, IN_MEMORY, ON_DISK
   char modified;    // modified flag
   int  frame;       // frame holding this page
   int  accessTime;  // clock tick for last access (most recently read/written)
   int  loadTime;    // clock tick for last time loaded (last loaded)
   int  nPeeks;      // #times read (how many times did we look at the page)
   int  nPokes;      // #times modified (how many times did we write data into the page in memory)
   Node n;           // access to the respective node in the list
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

// O(1) append new node to list tail
Node append(List l, int pno) {
   Node new = malloc(sizeof(node));
   new->pno = pno;
   new->prev = NULL;
   new->next = NULL;
   Node curr = l->tail;
   if (curr == NULL) {
      l->head = new;
      l->tail = new;
   } else {
      // link forward
      curr->next = new;
      l->tail = new;
      // link backward
      new->prev = curr;
   }
   return new;
}

// O(1) Place recently accessed page to tail of list
void updateLRUList(PTE *p) {
   // grab PTE respective node
   Node curr = p->n;
   // relink prev -> next nodes
   if (curr != LRU->head) {
      curr->prev->next = curr->next;
      curr->next->prev = curr->prev;
   } else {
      LRU->head = curr->next;
      LRU->head->prev = NULL;
   }
   // link to tail
   LRU->tail->next = curr;
   curr->prev = LRU->tail;
   curr->next = NULL;
   LRU->tail = curr;
}

/* --- MAIN PAGE TABLE PROGRAM --- */

/* Initialise Page Table data */
void initPageTable(int policy, int np)
{
   // init page replacement data structures
   if (policy == REPL_FIFO) {
      FIFO = newList();
   } else if (policy == REPL_LRU) {
      LRU = newList();
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
      p->n = NULL;
   }
}

/*  Request access to page pno in mode */
int requestPage(int pno, char mode, int time)
{
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
            // reset modified, fno, access/load times
            v->modified = 0;
            v->frame = NONE;
            v->accessTime = NONE;
            v->loadTime = NONE;
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
         // append new page to FIFO list
         if (replacePolicy == REPL_FIFO) {
            append(FIFO, pno);
         // append new page to LRU list + link PTE to LRU list node
         } else if (replacePolicy == REPL_LRU) {
            Node new = append(LRU, pno);
            p->n = new;
         }
         break;
      case IN_MEMORY:
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
      updateLRUList(p);
   }
   return p->frame;
}

/* Find victim using selected repl policy */
static int findVictim(int time)
{
   int victim = 0;
   switch (replacePolicy) {
   // O(1) grab head of LRU list
   case REPL_LRU:
      // get victim pno
      victim = LRU->head->pno;
      // link to new head
      Node currLru = LRU->head;
      Node prevLru = currLru;
      currLru = currLru->next;
      LRU->head = currLru;
      // free victim
      free(prevLru);
      break;
   // O(1) grab head of FIFO list
   case REPL_FIFO:
      // get victim pno
      victim = FIFO->head->pno;
      // link to new head
      Node curr = FIFO->head;
      Node prev = curr;
      curr = curr->next;
      FIFO->head = curr;
      // free victim
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
