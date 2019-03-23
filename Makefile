
CC = gcc

CFLAGS =-Wall -g -std=c11

LDLIBS= -lm

ALL = multicast

all : $(ALL)

multicast: multicast.o
multicast.o: multicast.h multicast.c
cleanall:
	rm -rf *~ $(ALL)
