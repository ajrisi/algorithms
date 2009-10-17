TARGET=libalgo.a

all: algo.h heap.o prng.o graph.o dqueue.o hashtable.o
	ar rcs $(TARGET) heap.o prng.o graph.o dqueue.o hashtable.o

heap.o:	heap.c heap.h
	gcc -ansi -Wall -o heap.o -c heap.c

dqueue.o: dqueue.h dqueue.c
	gcc -ansi -Wall -o dqueue.o -c dqueue.c

prng.o:	prng.c prng.h
	gcc -ansi -Wall -o prng.o -c prng.c

graph.o: graph.c graph.h
	gcc -ansi -Wall -o graph.o -c graph.c

hashtable.o: hashtable.h hashtable.c
	gcc -ansi -Wall -o hashtable.o -c hashtable.c

clean:	
	rm -f $(TARGET) 2> /dev/null
	rm -f *.o 2> /dev/null

distclean: clean
	rm -f *~ 2> /dev/null