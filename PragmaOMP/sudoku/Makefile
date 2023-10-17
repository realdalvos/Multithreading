CFLAGS = -std=c11 -g -O3 -Wall

ifeq ($(CC),icc)
# Intel compiler
OMP_FLAGS += -qopenmp
endif
ifeq ($(CC),gcc)
# GNU compiler
OMP_FLAGS += -fopenmp
endif


.PHONY: run compile clean

compile: sudoku_seq.exe sudoku_par.exe 

run: sudoku_seq.exe sudoku_par.exe
	./sudoku_seq.exe 9
	./sudoku_par.exe 9

clean::
	-rm -f sudoku_par.exe sudoku_seq.exe *.o

sudoku_par.exe: sudoku.c sudoku_par.c sudoku.h
	$(CC) $(CFLAGS) $(OMP_FLAGS) -o $@ $^ -lFHBRS -lm

sudoku_seq.exe: sudoku.c sudoku_seq.c sudoku.h
	$(CC) $(CFLAGS) -o $@ $^ -lFHBRS -lm
