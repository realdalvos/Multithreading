CC	= icc -qopenmp -fp-model strict
CFLAGS	= -O2 -Wall -no-multibyte-chars
LDFLAGS	= -lFHBRS -L/usr/X11R6/lib -lX11 -lpthread -lm

default:: mandelbrot.exe

clean::
	-rm -f mandelbrot.exe mandelbrot.o

# mit grafischer Anzeige
run:: mandelbrot.exe
	./mandelbrot.exe -2 -2 2 2 50000 1
# ohne grafische Anzeige
run2:: mandelbrot.exe
	./mandelbrot.exe -2 -2 2 2 50000 0

mandelbrot.exe: mandelbrot.o
	$(CC) -o $@ $< $(LDFLAGS)

mandelbrot.o: mandelbrot.c
	$(CC) $(CFLAGS) -c $<

