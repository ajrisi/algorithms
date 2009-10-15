TARGET=libalgo.a

all:	heap.o prng.o graph.o
	ar rcs $(TARGET) heap.o prng.o graph.o

heap.o:	heap.c heap.h
	gcc -ansi -Wall -o heap.o -c heap.c

prng.o:	prng.c prng.h
	gcc -ansi -Wall -o prng.o -c prng.c

graph.o:	graph.c graph.h
	gcc -ansi -Wall -o graph.o -c graph.c

clean:	
	rm -f $(TARGET) 2> /dev/null
	rm -f *.o 2> /dev/null

distclean:	clean
	rm -f *~ 2> /dev/null