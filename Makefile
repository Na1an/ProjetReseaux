
CC = gcc

CFLAGS =-Wall -g -std=c11

LDLIBS= -lm

ALL = pair

all : $(ALL)

list.o : include.h list.h list.c

tlvFct.o : include.h tlvFct.h tlvFct.c

pair.o : include.h tlvFct.h list.h pair.h pair.c

pair : list.o tlvFct.o pair.o

cleanall:
	rm -rf *~ $(ALL)
