TARGET=libalgo.a

all:	minheap.o
	ar rcs $(TARGET) minheap.o

minheap.o:	minheap.c minheap.h
	gcc -o minheap.o -c minheap.c

clean:	
	rm -f $(TARGET) 2> /dev/null
	rm -f *.o 2> /dev/null

dist-clean:	clean
	rm -f *~ 2> /dev/null