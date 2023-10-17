# module load gcc necessary
CC	= gcc
CFLAGS	= -g -Wall -std=c11 -fopenmp
LDFLAGS	= -fopenmp -lFHBRS -lX11 -lpthread -lm
HOST    = $(shell hostname)


default:: vector.exe

clean::
	-rm -f *.exe *.o

# production run (large arrays)
run:: vector.exe
ifeq ($(HOST),wr0)
	echo "not allowed on wr0!"
else
	./$< 1000000000 256
endif

# small test run
test: vector.exe
	./$< 10 8

vector.exe: vector.o
	$(CC) -o $@ $< $(LDFLAGS)

vector.o: vector.c
	$(CC) $(CFLAGS) -c $<

