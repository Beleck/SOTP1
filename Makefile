CC=gcc
CFLAGS=-g -Wall
IDIR=include
DEPS=$(IDIR)/viewer.h $(IDIR)/slave.h $(IDIR)/application.h
OBJ=viewer.o slave.o application.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%: %.o
	$(CC) -o $@ $^ $(CFLAGS)

all: application slave 
