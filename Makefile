TARGET=libalgo.a

all:	heap.o prng.o
	ar rcs $(TARGET) heap.o prng.o

heap.o:	heap.c heap.h
	gcc -ansi -Wall -o heap.o -c heap.c

prng.o:	prng.c prng.h
	gcc -ansi -Wall -o prng.o -c prng.c

clean:	
	rm -f $(TARGET) 2> /dev/null
	rm -f *.o 2> /dev/null

dist-clean:	clean
	rm -f *~ 2> /dev/null