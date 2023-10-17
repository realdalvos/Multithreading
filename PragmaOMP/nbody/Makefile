# Intel compiler
CC	= icc -qopenmp -fp-model strict
CFLAGS	= -O2 -std=c99 -Wall
LDLIBS	= -lFHBRS -lX11 -lpthread -lm


default:: nbody.exe

run:: nbody.exe
	./nbody.exe -t_end 36000000 -timesteps 10000 -bodies 1000

run2:: nbody.exe
	./nbody.exe -t_end 36000000 -timesteps 10000 -bodies 1000 -nodisplay

clean::
	-rm -f *.exe *.o


nbody.exe: nbody.o
	$(CC) -o $@ $< $(LDLIBS)

nbody.o: nbody.c
	$(CC) $(CFLAGS) -c $<

