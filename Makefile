TARGET=libalgo.a

all:	minheap.o
	ar rcs $(TARGET) minheap.o

minheap.o:	minheap.c minheap.h
	gcc -o minheap.o -c minheap.c
