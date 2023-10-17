/*==============================================================================
  
   Purpose          : solve sudoku by brute force
   Author           : Rudolf Berrendorf
                      Computer Science Department
                      Bonn-Rhein-Sieg University of Applied Sciences
	              53754 Sankt Augustin, Germany
                      rudolf.berrendorf@h-brs.de
  
==============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <omp.h>

#include "sudoku.h"


//==============================================================================
/** read test data from file
 * @param n size of (square sized) board
 * @param board sudoku board
 */
void initBoard(int n, int board[n][n]) {
  // read fixed test case for that problem size
  char filename[64];
  sprintf(filename, "testdata/case_%02d.txt", n);
  FILE *f = fopen(filename, "r");
  if(f == NULL) {
    printf("can not open file %s\n", filename);
    exit(1);
  }

  // read all data
  for(int i=0; i<n; i++) {
    for(int j=0; j<n; j++) {
      int rc = fscanf(f, "%d", &board[i][j]);
      if(rc != 1) {
        printf("problem in reading data from file\n");
        exit(2);
      }
    }
  }

  fclose(f);
}


//==============================================================================
/** print a horizontal block ruler
 * @param n size of (square sized) board
 * @param root square root of n
 */
static void printBoardRuler(int n, int root) {
  const int size = (n < 10) ? 1 : 2;

  for(int j=0; j<n; j++) {
    // block splitter
    if(j % root == 0) {
      printf("|-");
    }

    for(int i=0; i<size+1; i++) {
      printf("-");      
    }
  }

  printf("|\n");
}


//==============================================================================
/** print board on display
 * @param n size of (square sized) board
 * @param board sudoku board
 */
void printBoard(int n, int board[n][n]) {
  // get block size (sqrt(n))
  const int root = (int)sqrt(n);
  // generate print format according to possible value sizes
  const int size = (n < 10) ? 1 : 2;
  char format1[10];
  char format2[10];
  // generate the format strings %1d / %1s or %2d / %2s
  sprintf(format1, "%%%dd ", size);
  sprintf(format2, "%%%ds ", size);

  for(int i=0; i<n; i++) {
    // block ruler?
    if(i % root == 0) {
      printBoardRuler(n, root);
    }

    // print one row
    for(int j=0; j<n; j++) {
      // vertical block splitter
      if(j % root == 0) {
        printf("| ");
      }

      // data
      if(board[i][j] == VALUE_EMPTY) {
        printf(format2, ".");
      } else {
        printf(format1, board[i][j]);
      }
    }
    printf("|\n");
  }

  // end block ruler
  printBoardRuler(n, root);
}


//==============================================================================
/** check whether a given board breaks any sudoku rule
 * @param n size of (square sized) board
 * @param board sudoku board
 * @return 1: everything OK, 0: violation
 */
int check(int n, int board[n][n]) {
  // get block size (sqrt(n))
  const int root = (int)sqrt(n);

  // for every fixed value: check consistency rules
  for(int i=0; i<n; i++) {
    for(int j=0; j<n; j++) {

      // get my value
      const int value = board[i][j];

      // ignore non-set values
      if(value == VALUE_EMPTY) {
        continue;
      }

      // rule 1: in same row not the same value twice
      for(int k=0; k<n; k++) {
        if((k != j) && (board[i][k] == value)) {
          return FAILED;
        }
      }

      // rule 2: in same column not the same value twice
      for(int k=0; k<n; k++) {
        if((k != i) && (board[k][j] == value)) {
          return FAILED;
        }
      }

      // rule 3: in the same block not the same value twice
      // get the block beginning
      int iBlock = (i / root) * root;
      int jBlock = (j / root) * root;
      for(int k=iBlock; k<iBlock+root; k++) {
        for(int l=jBlock; l<jBlock+root; l++) {
          if((k != i) && (l != j) && (board[k][l] == value)) {
            return FAILED;
          }
        }
      }
    }
  }

  // no violation of rules, therefore everything OK
  return SUCCESS;
}


/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
