CC=gcc
CFLAGS=-g -Wall
DEPS=viewer.h slave.h application.h
OBJ=viewer.o slave.o application.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%: %.o
	$(CC) -o $@ $^ $(CFLAGS)

all: application
