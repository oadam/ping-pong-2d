CC=g++
CFLAGS=-I.

ODIR=obj

VPATH= $(ODIR) src


LIBS=-lglut -lGL -lGLU

pingpong: moteur.o testApp.o
	$(CC) -o $@ $^ -I. $(LIBS)

test: 	moteurDebug.o testApp.o
	$(CC) -o $@ $^ -I. $(LIBS)

testApp.o: testApp.cpp
	$(CC) -c -o $@ $^ $(CFLAGS)
     

moteur.o: moteur.cpp moteur.h
	$(CC) -c -o $@ $< $(CFLAGS)

moteurDebug.o: moteur.cpp moteur.h
	$(CC) -c -o $@ $< $(CFLAGS) -DDEBUG 
  
.PHONY: clean

#clean:
#	rm -f *.o  

