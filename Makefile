TARGET=libalgo.a

INST_HEADERS= algo.h heap.h dqueue.h prng.h graph.h hashtable.h hash.h trie.h dictionary.h

all: algo.h heap.o prng.o graph.o dqueue.o hashtable.o hash.o trie.o dictionary.o
	ar rcs $(TARGET) heap.o prng.o graph.o dqueue.o hashtable.o hash.o trie.o dictionary.o

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

hash.o: hash.h hash.c
	gcc -ansi -Wall -DNETHASH -o hash.o -c hash.c

trie.o: trie.h trie.c
	gcc -ansi -Wall -o trie.o -c trie.c

dictionary.o: dictionary.h dictionary.c
	gcc -ansi -Wall -o dictionary.o -c dictionary.c

install:
	install -m 644 libalgo.a /usr/lib/
	mkdir /usr/include/algo ; true
	install -m 644 $(INST_HEADERS) /usr/include/algo

uninstall:
	rm /usr/lib/libalgo.a
	cd /usr/include/algo && rm $(INST_HEADERS)

clean:	
	rm -f $(TARGET) 2> /dev/null
	rm -f *.o 2> /dev/null

distclean: clean
	rm -f *~ 2> /dev/null