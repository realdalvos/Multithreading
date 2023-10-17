# Intel compiler
CC	= icc -qopenmp -fp-model strict
# sequentiell
#CC	= icc -fp-model strict

CFLAGS	= -O2 -g -Wall -std=c99
LDLIBS	= -lFHBRS -ltiff -lX11 -lpthread -lm


default:: image.exe

# mit grafischer Ausgabe
run:: image.exe
	./image.exe Test.tiff

# ohne grafische Ausgabe
run2:: image.exe
	./image.exe -n Test.tiff

clean::
	-rm -f *.exe *.o

image.exe: main.o tiff.o rgb.o gray.o edgedetect.o
	$(CC) -o $@ $^ $(LDLIBS)

edgedetect.o: edgedetect.c edgedetect.h commondefs.h gray.h rgb.h
gray.o: gray.c commondefs.h rgb.h gray.h
main.o: main.c tiff.h rgb.h gray.h commondefs.h edgedetect.h
rgb.o: rgb.c commondefs.h rgb.h gray.h
tiff.o: tiff.c tiff.h rgb.h gray.h commondefs.h
