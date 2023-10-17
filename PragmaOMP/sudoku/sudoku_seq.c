/*==============================================================================
  
   Purpose          : solve sudoku by brute force (sequential)
   Author           : Rudolf Berrendorf
                      Computer Science Department
                      Bonn-Rhein-Sieg University of Applied Sciences
	              53754 Sankt Augustin, Germany
                      rudolf.berrendorf@h-brs.de
  
==============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <libFHBRS.h>

#include "sudoku.h"


//==============================================================================
/** solve a sudoku with backtracking (brute force)
 * @param n size of (square sized) board
 * @param board sudoku board
 * @param row current row position
 * @param column current column position
 * @return 1 success, 0: no feasible solution found
 */
int solve(int n, int board[n][n], int row, int column) {

  // check if we reached the end position (lower left corner)
  if((row == n-1) && (column == n)) {
    // all values in board set, return success
    return SUCCESS;
  }

  // check if we reached the last column, then advance to next row and restart column
  if(column == n) {
    column = 0;
    row++;
  }

  // value at current position is already set?
  if(board[row][column] != VALUE_EMPTY) {
    // yes, then advance in column
    return solve(n, board, row, column + 1);
  }


  // at the current position the value is free, try all possible values
  for(int value=1; value <= n; value++) {

    // place one value at this position
    board[row][column] = value;

    // check whether this is a feasible value at this position
    if(check(n, board) == SUCCESS) {
      
      // solve recursively
      if(solve(n,  board, row, column+1) == SUCCESS) {
        // we found a solution
        return SUCCESS;
      }

    } // check()

    // at this point, either the value was not allowed here or we got no
    // solution with this value
    // backtracking: remove value
    board[row][column] = VALUE_EMPTY;
  } // for


  // did not succeed
  return FAILED;
}


//==============================================================================

int main(int argc, char **argv) {
  
  // size of sudoku
  const int n = atoi(argv[1]);
  // allocate board, initialize it and print the
  // initial board on the display
  int board[n][n];
  initBoard(n, board);
  printf("Running sequentially. Initial board (n=%d):\n", n);
  printBoard(n, board);
  
  // solve the sudoku
  int rc;
  double t = gettime();
  // find a solution for the sudoku
  rc = solve(n, board, 0, 0);
  t = gettime() - t;
  
  if(rc == SUCCESS) {
    printf("\nFinal solution:\n");
    printBoard(n, board);
    printf("Sudoku solved in time %.6f s!\n", t);
  } else {
    printf("Could not find any solution in %.6f time!\n", t);
  }
  
  return 0;
}

/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
