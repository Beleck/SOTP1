CC=gcc
CFLAGS=-g -Wall -Wextra -std=c99
LDFLAGS=-pthread -lrt
IDIR=include
DEPS=$(wildcard $(IDIR)/*.h)
OBJ=app_shm.o
TARGET=application slave viewer

all: $(TARGET)

application: application.o $(OBJ)
viewer: viewer.o $(OBJ)
slave: slave.o $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $^

%.o: %.c $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY = clean
clean:
	rm -f $(TARGET) 

