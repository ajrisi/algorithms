TARGET=libalgo.a

all:	minheap.o prng.o
	ar rcs $(TARGET) minheap.o prng.o

minheap.o:	minheap.c minheap.h
	gcc -ansi -Wall -o minheap.o -c minheap.c

prng.o:	prng.c prng.h
	gcc -ansi -Wall -o prng.o -c prng.c

clean:	
	rm -f $(TARGET) 2> /dev/null
	rm -f *.o 2> /dev/null

dist-clean:	clean
	rm -f *~ 2> /dev/null