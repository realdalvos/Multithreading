/*==============================================================================
  
   Purpose          : definitions for sudoku
   Author           : Rudolf Berrendorf
                      Computer Science Department
                      Bonn-Rhein-Sieg University of Applied Sciences
	              53754 Sankt Augustin, Germany
                      rudolf.berrendorf@h-brs.de
  
==============================================================================*/

#if !defined(SUDOKU_H_INCLUDED)
#define SUDOKU_H_INCLUDED


// value of a board cell that marks the cell as free
#define VALUE_EMPTY 0

// return values for functions check/solve
#define SUCCESS 1
#define FAILED 0


// functions need in other files
extern void initBoard(int n, int board[n][n]);
extern void printBoard(int n, int board[n][n]);
extern int solve(int n, int board[n][n], int row, int column);
extern int check(int n, int board[n][n]);

#endif // !defined(SUDOKU_H_INCLUDED)

/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
