
CC = gcc

CFLAGS =-Wall -g -std=c11

LDLIBS= -lm

ALL = multicast

all : $(ALL)

multicast: multicast.o
multicast.o: multicast.h multicast.c

tlvFct.o : tlvFct.c tlvFct.h

pair.o : pair.c pair.h tlvFct.h

pair : tlvFct.o pair.o

cleanall:
	rm -rf *~ $(ALL)
