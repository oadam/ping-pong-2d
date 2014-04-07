CC=g++
CFLAGS=-I.

VPATH= $(ODIR) src


LIBS=-lglut -lGL -lGLU

pingpong: moteur.o testApp.o
	$(CC) -o $@ $^ -I. $(LIBS)

test: moteurDebug.o testApp.o
	$(CC) -o $@ $^ -I. $(LIBS)

testApp.o: src/testApp.cpp
	$(CC) -c -o $@ $^ $(CFLAGS)
     

moteur.o: src/moteur.cpp src/moteur.h
	$(CC) -c -o $@ $< $(CFLAGS)

moteurDebug.o: src/moteur.cpp src/moteur.h
	$(CC) -c -o $@ $< $(CFLAGS) -DDEBUG 
  
.PHONY: clean

clean:
	rm -f *.o 

