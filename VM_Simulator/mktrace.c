// mktrace.c ... generate "random" page reference traces
// COMP1521 17s2 Assignment 2
// Written by John Shepherd

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
   int nPages, nReqs;

   if (argc < 3) {
      printf("Usage: %s #Requests #Pages\n",argv[0]);
      return EXIT_FAILURE;
   }
   nReqs = atoi(argv[1]);
   if (nReqs < 10) {
      printf("Invalid #Requests (must be number >= 10)\n");
      return EXIT_FAILURE;
   }

   nPages = atoi(argv[2]);
   if (nPages < 4) {
      printf("Invalid #Pages (must be number >= 4)\n");
      return EXIT_FAILURE;
   }
   
   int page;      // current page to be referenced
   int prev = 0;  // previous page referenced
   char req;      // request type: read/write
   for (int i = 0; i < nReqs; i++) {
      req = (random()%10 < 3) ? 'w' : 'r';
      switch (random()%10) {
      case 0: case 1: case 2:
         // simulates working set by favouring current page
         page = prev;
         break;
      case 3:
         page = prev+1;
         break;
      case 4:
         page = prev-1;
         break;
      default:
         page = random()%nPages;
         break;
      }
      if (page < 0) page = 0;
      if (page > nPages-1) page = nPages-1;
      printf("%c%d",req,page);
      prev = page;
      printf("%c", (i % 20 == 19) ? '\n' : ' ');
   }
   printf("\n");
   return EXIT_SUCCESS;
}
