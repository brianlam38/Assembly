#include <stdio.h>
#include <stdlib.h>

#include "board1.h"

int neighbours(int, int);
void copyBackAndShow();

int main(void)
{
    int maxiters;
    printf("# Iterations: ");
    scanf("%d", &maxiters);                   // input number of iterations
    for (int n = 1; n <= maxiters; n++) {     // for each iteration
      for (int i = 0; i < N; i++) {           //      for each row
         for (int j = 0; j < N; j++) {        //          for each col
            int nn = neighbours(i,j);         //            [LIVING CELL]
            if (board[i][j] == 1) {           //            if curr cell = 1
               if (nn < 2)                    //                if neighbours < 2  (underpop: cells with nn < 2 dies)
                  newboard[i][j] = 0;         //                    updated = 0
               else if (nn ==2 || nn == 3)    //                else if neighbours = 2 or 3 (lives: cells nn = 2 | 3)
                  newboard[i][j] = 1;         //                    updated = 1
               else                           //                else                (overpop: cells with nn > 3)
                  newboard[i][j] = 0;         //                    updated = 0
            }                                 //            [DEAD CELL]
            else if (nn == 3)                 //            else if curr neighbours = 3  (reproduction)
               newboard[i][j] = 1;            //                updated = 1
            else                              //            else                    (no reproduction)
               newboard[i][j] = 0;            //                updated = 0
         }
      }
      printf("=== After iteration %d ===\n", n);  // print iteration #
      copyBackAndShow();                          // show updated board for current iteration
   }
   return 0;
}

int neighbours(int i, int j)
{
    int nn = 0;                                   // nn = 0
   for (int x = -1; x <= 1; x++) {                // for each x direction
      for (int y = -1; y <= 1; y++) {             //    for each y direction
         if (i+x < 0 || i+x > N-1) continue;      //         at edge -> skip
         if (j+y < 0 || j+y > N-1) continue;      //         at edge -> skip
         if (x == 0 && y == 0) continue;          //         itself -> skip
         if (board[i+x][j+y] == 1) nn++;          //         neighbour exists -> nn++
        }                                         //
    }                                             //
   return nn;                                     // return num neighbours
}

void copyBackAndShow()
{
   for (int i = 0; i < N; i++) {          // for each col
      for (int j = 0; j < N; j++) {       //      for each row
         board[i][j] = newboard[i][j];    //          cell = new_cell
         if (board[i][j] == 0)            //          if cell = 0
            putchar('.');                 //              put '.'
         else                             //          else
            putchar('#');                 //              put #
      }                                   //      put newline
      putchar('\n');                      //
   }
}
