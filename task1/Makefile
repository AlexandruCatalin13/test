CC=gcc
CFLAGS=-I. -Wall -pthread
DEPS = list.h thread_work.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

build: *.o 
	$(CC) -g -o task1 *.o $(CFLAGS)
