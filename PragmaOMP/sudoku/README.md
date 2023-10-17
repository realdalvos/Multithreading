Solve a square sized soduko using brute force backtracking.
Available sizes are 4x4, 9x9 and 16x16.


Do once:
    module load intel-compiler libFHBRS


To compile:
   make

To run on the 9x9 example problem:
   export OMP_NUM_THREADS=4
   make run

or manually:
   ./sudoku.exe <n>
where n in {4,9,16} is the problem size.


Source files:
sudoku.h: definitions
sudoku.c: helper functions
sudoku_seq.c: sequential algorithm in function solve and main
sudoku_par.c: parallel algorithm in function solve and main
