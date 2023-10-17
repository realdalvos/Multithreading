CC		= mpicc
CFLAGS		= -std=c99 -O2 -g -Wall
# with debug output enabled
#CFLAGS		= -std=c99 -O2 -g -Wall -DDEBUG
LDLIBS		= -lFHBRS -lm

EXE	= sort.exe

########################################################################

default:: compile

compile:: $(EXE)

clean::
	-rm -f *.exe *.o

run:: $(EXE)
	mpirun -np 2 $(EXE) 6


########################################################################

%.o: %.c
	$(CC) $(CFLAGS) -c $<
%.exe: %.o
	$(CC) -o $@ $< $(LDLIBS)

########################################################################


