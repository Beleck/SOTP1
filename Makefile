CC=gcc
CFLAGS=-g -Wall -Wextra -std=c99
LDFLAGS=-pthread -lrt
IDIR=include
DEPS=$(wildcard $(IDIR)/*.h)
OBJ=app_shm.o app_signal.o app_file.o
TARGET=application slave viewer

all: $(TARGET)
	rm *.o

application: application.o $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $^ 

viewer: viewer.o $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $^ 

slave: slave.o $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $^ 

%.o: %.c $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY = clean
clean:
	rm -f $(TARGET) *.o
