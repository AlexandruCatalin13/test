CC=gcc
CFLAGS=-I. -Wall -lpthread
DEPS = list.h thread_work.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

build: list.o task1.o thread_work.o
	$(CC) -o task1 task1.o list.o thread_work.o $(CFLAGS)
