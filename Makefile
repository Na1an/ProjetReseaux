
CC = gcc

CFLAGS =-Wall -g -std=c11

LDLIBS= -lm

ALL = pair

all : $(ALL)

list.o : list.c list.h

tlvFct.o : tlvFct.c tlvFct.h

pair.o : pair.c pair.h tlvFct.h

pair : list.o tlvFct.o pair.o

cleanall:
	rm -rf *~ $(ALL)
