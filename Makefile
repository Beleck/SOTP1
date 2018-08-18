CC=gcc
CFLAGS=-g -Wall -Wextra -std=c99
LDFLAGS=-pthread -lrt
IDIR=include
DEPS=$(IDIR)/viewer.h $(IDIR)/slave.h $(IDIR)/application.h
OBJ=viewer.o slave.o application.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%: %.o
	$(CC) -o $@ $^ $(LDFLAGS)

all: application slave viewer

.PHONY = clean

clean:
	rm -f application.o slave.o application slave viewer
