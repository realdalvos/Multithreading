# module load gcc necessary
CC	= gcc
CFLAGS	= -g -Wall -std=c11
LDFLAGS	= -lFHBRS -lX11 -lpthread -lm
HOST    = $(shell hostname)


default:: vector.exe

clean::
	-rm -f *.exe *.o

# production run (large arrays)
run:: vector.exe
ifeq ($(HOST),wr0)
	echo "not allowed on wr0!"
else
	./$< 10000000000 64
endif

# small test run
test: vector.exe
	./$< 10 16

vector.exe: vector.o
	$(CC) -o $@ $< $(LDFLAGS)

vector.o: vector.c
	$(CC) $(CFLAGS) -c $<

