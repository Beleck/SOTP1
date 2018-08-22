CC=gcc
CFLAGS=-g -Wall -Wextra -std=c99 -Iinclude
LDFLAGS=-pthread -lrt
IDIR=include
DEPS=$(wildcard $(IDIR)/*.h)
OBJDIR=obj
OBJ=$(OBJDIR)/app_shm.o $(OBJDIR)/app_signal.o $(OBJDIR)/tools.o
TARGET=application slave viewer

all: $(TARGET)

application: $(OBJDIR)/application.o $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $^ 

viewer: $(OBJDIR)/viewer.o $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $^ 

slave: $(OBJDIR)/slave.o $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $^ 

$(OBJDIR)/%.o: %.c $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: test
test: all
	@cd test && bash script_test.sh

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJDIR)/*
